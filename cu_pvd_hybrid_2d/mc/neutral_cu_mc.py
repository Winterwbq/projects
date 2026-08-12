from __future__ import annotations

from dataclasses import dataclass

import numpy as np

from fields.interpolation import CartesianMesh2D
from mc.boundaries import ChamberGeometry
from mc.particles import CU_MASS_KG, speed_from_ev
from mc.tallies import NeutralMCResult


@dataclass
class NeutralCuMC:
    """Ballistic Monte Carlo transport for neutral Cu atoms in 2D Cartesian XY.

    Particles are emitted from the target (y=0) with a 2D cosine distribution
    and tracked on straight-line paths until they hit a boundary.  The density
    tally uses the particle's residence time in each cell.
    """

    mesh: CartesianMesh2D
    geometry: ChamberGeometry
    config: dict
    rng: np.random.Generator

    def run(self, gamma_target_x: np.ndarray) -> NeutralMCResult:
        """Run neutral Cu MC given the target flux profile (shape nx,).

        Parameters
        ----------
        gamma_target_x:
            Cu neutral flux density at the target, m⁻¹ s⁻¹, shape (nx,).
        """
        n_particles = int(self.config.get("mc", {}).get("n_particles_neutral", 10000))
        ds = float(self.config.get("mc", {}).get("neutral_step", min(self.mesh.dx, self.mesh.dy)))
        energy_ev = float(self.config.get("physics", {}).get("cu_neutral_energy_eV", 3.0))

        source_rate = _integrate_linear_flux(self.mesh, gamma_target_x)
        if source_rate <= 0.0:
            return NeutralMCResult(
                n_cu=np.zeros(self.mesh.shape),
                gamma_wafer=np.zeros(self.mesh.nx),
                gamma_wall_rate=0.0,
                gamma_target_return=np.zeros(self.mesh.nx),
                total_source_rate=0.0,
            )

        particle_rate = source_rate / n_particles
        density_residence = np.zeros(self.mesh.shape)
        wafer_flux = np.zeros(self.mesh.nx)
        target_return = np.zeros(self.mesh.nx)
        wall_rate = 0.0

        launch_x = _sample_linear_source(self.rng, self.mesh, gamma_target_x, n_particles)
        dirs = _sample_cosine_2d(self.rng, n_particles)  # shape (n, 2): (dx/ds, dy/ds)
        speed = float(speed_from_ev(energy_ev, CU_MASS_KG))
        dt = ds / max(speed, 1.0e-30)

        for i in range(n_particles):
            xp = launch_x[i]
            yp = 1.0e-9          # just above target surface
            vx, vy = dirs[i]     # unit direction in 2D

            for _ in range(20000):
                hit = self.geometry.classify(xp, yp)
                if hit:
                    if hit == "wafer":
                        _add_linear_flux(self.mesh, wafer_flux, xp, particle_rate)
                    elif hit == "target":
                        _add_linear_flux(self.mesh, target_return, xp, particle_rate)
                    else:
                        wall_rate += particle_rate
                    break

                ix, iy = _cell_index(self.mesh, xp, yp)
                if ix is not None:
                    density_residence[ix, iy] += particle_rate * dt

                xp += vx * ds
                yp += vy * ds

        n_cu = density_residence / self.mesh.cell_volumes()
        return NeutralMCResult(
            n_cu=n_cu,
            gamma_wafer=wafer_flux,
            gamma_wall_rate=wall_rate,
            gamma_target_return=target_return,
            total_source_rate=source_rate,
        )


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------

def _sample_cosine_2d(rng: np.random.Generator, n: int) -> np.ndarray:
    """Sample directions from a 2D cosine (Lambertian) emission distribution.

    In 2D the cosine law gives P(θ) ∝ cos θ on [-π/2, π/2].
    Integrating: sin θ is uniform on [-1, 1], so sin θ = 2u - 1.
    Particles are emitted into the upper half-plane (vy > 0).
    Returns shape (n, 2): each row is a unit direction (vx/v, vy/v).
    """
    u = rng.uniform(0.0, 1.0, size=n)
    sin_theta = 2.0 * u - 1.0                                      # ∈ (-1, 1)
    cos_theta = np.sqrt(np.maximum(1.0 - sin_theta ** 2, 0.0))    # vy/v ≥ 0
    return np.column_stack([sin_theta, cos_theta])


def _sample_linear_source(
    rng: np.random.Generator,
    mesh: CartesianMesh2D,
    gamma_x: np.ndarray,
    n: int,
) -> np.ndarray:
    """Sample launch x-positions weighted by the target flux profile.

    Weights = gamma_x * dx (particle rate per cell), then sample uniformly
    within the chosen cell.
    """
    dx = mesh.surface_areas_bottom()                     # shape (nx,)
    weights = np.maximum(np.asarray(gamma_x, dtype=float), 0.0) * dx
    total = float(np.sum(weights))
    if total <= 0.0:
        raise ValueError("target source map has zero total flux")
    cdf = np.cumsum(weights) / total
    cells = np.searchsorted(cdf, rng.uniform(0.0, 1.0, size=n), side="right")
    cells = np.clip(cells, 0, mesh.nx - 1)
    x0 = mesh.x_edges[cells]
    x1 = mesh.x_edges[cells + 1]
    return x0 + rng.uniform(0.0, 1.0, size=n) * (x1 - x0)


def _integrate_linear_flux(mesh: CartesianMesh2D, gamma_x: np.ndarray) -> float:
    """Total particle rate from target: sum(gamma * dx) [s⁻¹ per unit z-depth]."""
    return float(np.sum(np.maximum(gamma_x, 0.0) * mesh.surface_areas_bottom()))


def _add_linear_flux(mesh: CartesianMesh2D, gamma_x: np.ndarray, x: float, rate: float) -> None:
    """Accumulate a particle hit at position x into the flux array."""
    idx = np.searchsorted(mesh.x_edges, x, side="right") - 1
    if 0 <= idx < mesh.nx:
        gamma_x[idx] += rate / mesh.surface_areas_bottom()[idx]


def _cell_index(mesh: CartesianMesh2D, x: float, y: float) -> tuple[int | None, int | None]:
    if (
        x < mesh.x_edges[0] or x >= mesh.x_edges[-1]
        or y < mesh.y_edges[0] or y >= mesh.y_edges[-1]
    ):
        return None, None
    ix = int(np.clip(np.searchsorted(mesh.x_edges, x, side="right") - 1, 0, mesh.nx - 1))
    iy = int(np.clip(np.searchsorted(mesh.y_edges, y, side="right") - 1, 0, mesh.ny - 1))
    return ix, iy
