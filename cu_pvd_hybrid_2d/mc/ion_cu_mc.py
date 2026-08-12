from __future__ import annotations

from dataclasses import dataclass

import numpy as np

from fields.bmap_reader import BMap
from fields.interpolation import CartesianMesh2D
from fields.zapdos_field_reader import ZapdosFields
from mc.boris import boris_push
from mc.boundaries import ChamberGeometry
from mc.particles import CU_MASS_KG, ELEMENTARY_CHARGE, EV_TO_J, sample_maxwellian_velocity
from mc.tallies import IonMCResult


@dataclass
class IonCuMC:
    """Boris-pushed Cu+ ion Monte Carlo in 2D Cartesian (2.5D velocity space).

    Position is tracked in 2D (x, y).  Velocity is 3D (vx, vy, vz) because
    the in-plane B-field (Bx, By, 0) drives a v×B force that generates an
    out-of-plane vz component.  vz does not affect position.

    Fields from ZapdosFields.e_at and BMap.at already return 3D Cartesian
    arrays (Ex, 0, Ey) and (Bx, By, 0) respectively, so no cylindrical
    conversion is needed.
    """

    mesh: CartesianMesh2D
    geometry: ChamberGeometry
    config: dict
    rng: np.random.Generator

    def run(self, s_iz: np.ndarray, fields: ZapdosFields, b_total: BMap) -> IonMCResult:
        n_particles = int(self.config.get("mc", {}).get("n_particles_ion", 8000))
        dt = float(self.config.get("mc", {}).get("ion_dt", 2.0e-9))
        max_steps = int(self.config.get("mc", {}).get("ion_max_steps", 2500))
        ion_temp = float(self.config.get("physics", {}).get("ion_temperature_eV", 0.15))

        total_birth = float(np.sum(np.maximum(s_iz, 0.0) * self.mesh.cell_volumes()))
        if total_birth <= 0.0:
            return _empty_result(self.mesh)

        # Birth positions (2D) and 3D velocities
        x_pos, y_pos = _sample_volume_source(self.rng, self.mesh, s_iz, n_particles)
        vel = sample_maxwellian_velocity(self.rng, n_particles, ion_temp, CU_MASS_KG)
        weights = np.full(n_particles, total_birth / n_particles)
        alive = np.ones(n_particles, dtype=bool)

        gamma_target = np.zeros(self.mesh.nx)
        gamma_wafer = np.zeros(self.mesh.nx)
        wall_rate = 0.0
        wafer_energy: list[float] = []
        wafer_angle: list[float] = []
        sheath_samples: list[tuple[float, float]] = []

        q_over_m = ELEMENTARY_CHARGE / CU_MASS_KG

        for _ in range(max_steps):
            idx = np.where(alive)[0]
            if idx.size == 0:
                break

            xi = x_pos[idx]
            yi = y_pos[idx]

            # Fields at current 2D positions — already 3D Cartesian vectors
            # e_at returns (Ex, 0, Ey) — note the ordering convention used by
            # ZapdosFields: axis-0 = x, axis-1 = z(out-of-plane), axis-2 = y
            # We re-pack to (Ex, Ey_zapdos, 0) → (Ex, 0, Ey) as (Fx, Fy, Fz)
            # with Fz = 0 so only x and y positions are advanced.
            e_cart = fields.e_at(xi, yi)   # shape (n, 3): [Ex, 0, Ey]
            b_cart = b_total.at(xi, yi)    # shape (n, 3): [Bx, By, 0]

            vel[idx] = boris_push(vel[idx], e_cart, b_cart, q_over_m, dt)

            x_pos[idx] += vel[idx, 0] * dt   # vx moves x
            y_pos[idx] += vel[idx, 2] * dt   # vz in the (Ex,0,Ey) convention is vy_physical

            # Note on axis convention:
            # e_at / b_total.at pack as [x-component, ignored, y-component]
            # boris_push works in 3D Cartesian; vel[:,0]=vx, vel[:,1]=v_out-of-plane, vel[:,2]=vy
            # Position update uses vel[:,0] for x and vel[:,2] for y.

            x_new = x_pos[idx]
            y_new = y_pos[idx]

            hit_target = (y_new <= 0.0) & (np.abs(x_new) <= self.geometry.target_radius)
            hit_wafer = (y_new >= self.geometry.target_to_wafer) & (np.abs(x_new) <= self.geometry.wafer_radius)
            hit_wall = (
                (np.abs(x_new) >= self.geometry.chamber_radius)
                | ((y_new >= self.geometry.target_to_wafer) & ~hit_wafer)
                | ((y_new <= 0.0) & ~hit_target)
            )

            for local, global_i in enumerate(idx):
                if hit_target[local]:
                    _add_linear_flux(self.mesh, gamma_target, x_new[local], weights[global_i])
                    alive[global_i] = False
                elif hit_wafer[local]:
                    _add_linear_flux(self.mesh, gamma_wafer, x_new[local], weights[global_i])
                    speed2 = float(np.dot(vel[global_i], vel[global_i]))
                    energy_ev = 0.5 * CU_MASS_KG * speed2 / EV_TO_J
                    # Normal-to-wafer speed is vy_physical = vel[:,2]
                    normal_speed = abs(float(vel[global_i, 2]))
                    angle = np.degrees(
                        np.arccos(np.clip(normal_speed / max(np.sqrt(speed2), 1.0e-30), 0.0, 1.0))
                    )
                    wafer_energy.append(energy_ev)
                    wafer_angle.append(angle)
                    if y_pos[global_i] > 0.95 * self.geometry.target_to_wafer:
                        sheath_samples.append((energy_ev, angle))
                    alive[global_i] = False
                elif hit_wall[local]:
                    wall_rate += float(weights[global_i])
                    alive[global_i] = False

        wall_rate += float(np.sum(weights[alive]))
        return IonMCResult(
            gamma_target=gamma_target,
            gamma_wafer=gamma_wafer,
            gamma_wall_rate=wall_rate,
            iedf_wafer_eV=np.asarray(wafer_energy, dtype=float),
            angle_wafer_deg=np.asarray(wafer_angle, dtype=float),
            sheath_edge_energy_angle=np.asarray(sheath_samples, dtype=float),
            total_birth_rate=total_birth,
        )


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------

def _sample_volume_source(
    rng: np.random.Generator,
    mesh: CartesianMesh2D,
    source: np.ndarray,
    n: int,
) -> tuple[np.ndarray, np.ndarray]:
    """Sample birth positions (x, y) weighted by the volume ionization source.

    In 2D Cartesian, cell area = dx * dy, so positions are sampled uniformly
    within each selected cell (no sqrt-r transform needed).
    """
    weights = np.maximum(source, 0.0) * mesh.cell_volumes()
    flat = weights.ravel()
    total = float(np.sum(flat))
    if total <= 0.0:
        raise ValueError("ionization source integrates to zero — cannot sample birth positions")
    cdf = np.cumsum(flat) / total
    flat_cells = np.searchsorted(cdf, rng.uniform(0.0, 1.0, size=n), side="right")
    flat_cells = np.clip(flat_cells, 0, mesh.nx * mesh.ny - 1)
    ix, iy = np.unravel_index(flat_cells, mesh.shape)

    x = mesh.x_edges[ix] + rng.uniform(0.0, 1.0, size=n) * (mesh.x_edges[ix + 1] - mesh.x_edges[ix])
    y = mesh.y_edges[iy] + rng.uniform(0.0, 1.0, size=n) * (mesh.y_edges[iy + 1] - mesh.y_edges[iy])
    return x, y


def _add_linear_flux(
    mesh: CartesianMesh2D,
    gamma_x: np.ndarray,
    x: float,
    rate: float,
) -> None:
    """Accumulate a surface hit at x into a per-cell flux array (nx,)."""
    idx = np.searchsorted(mesh.x_edges, x, side="right") - 1
    if 0 <= idx < mesh.nx:
        gamma_x[idx] += rate / mesh.surface_areas_bottom()[idx]


def _empty_result(mesh: CartesianMesh2D) -> IonMCResult:
    return IonMCResult(
        gamma_target=np.zeros(mesh.nx),
        gamma_wafer=np.zeros(mesh.nx),
        gamma_wall_rate=0.0,
        iedf_wafer_eV=np.zeros(0),
        angle_wafer_deg=np.zeros(0),
        sheath_edge_energy_angle=np.zeros((0, 2)),
        total_birth_rate=0.0,
    )
