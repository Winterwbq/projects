from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

import numpy as np

from fields.interpolation import CartesianMesh2D, Field2D, gradient_2d


@dataclass
class ZapdosFields:
    """Fluid plasma fields on the 2D Cartesian mesh returned by Zapdos.

    Electric field components are stored as Ex (x-direction) and Ey
    (y-direction, target→wafer).  There is no out-of-plane Ez component
    in the 2D model.
    """

    mesh: CartesianMesh2D
    phi: np.ndarray
    ex: np.ndarray
    ey: np.ndarray
    ne: np.ndarray
    te: np.ndarray
    s_iz_zapdos: np.ndarray | None = None

    def field(self, name: str) -> Field2D:
        return Field2D(self.mesh, getattr(self, name), name=name)

    def e_at(self, x: np.ndarray | float, y: np.ndarray | float) -> np.ndarray:
        """Return (Ex, Ey, 0) at query points, shape (n, 3)."""
        ex = self.field("ex").at(x, y)
        ey = self.field("ey").at(x, y)
        zeros = np.zeros_like(ex)
        return np.stack([ex, zeros, ey], axis=-1)


def read_zapdos_csv(path: str | Path, mesh: CartesianMesh2D) -> ZapdosFields:
    """Read cell-centred Zapdos fields from CSV columns x,y,phi,ne,Te or mean_energy."""

    data = np.genfromtxt(Path(path), delimiter=",", names=True)
    phi = _grid_column(data, "phi", mesh)
    ne = _grid_column(data, "ne", mesh)
    if "Te" in data.dtype.names:
        te = _grid_column(data, "Te", mesh)
    else:
        # Zapdos mean_energy is electron mean energy; for a Maxwellian,
        # mean energy = 3/2 Te, both in eV.
        te = (2.0 / 3.0) * _grid_column(data, "mean_energy", mesh)
    if "Ex" in data.dtype.names and "Ey" in data.dtype.names:
        ex = _grid_column(data, "Ex", mesh)
        ey = _grid_column(data, "Ey", mesh)
    else:
        dphi_dx, dphi_dy = gradient_2d(mesh, phi)
        ex = -dphi_dx
        ey = -dphi_dy
    s_iz = _grid_column(data, "S_iz_Zapdos", mesh) if "S_iz_Zapdos" in data.dtype.names else None
    return ZapdosFields(mesh=mesh, phi=phi, ex=ex, ey=ey, ne=ne, te=te, s_iz_zapdos=s_iz)


def synthetic_zapdos_solution(
    mesh: CartesianMesh2D,
    config: dict,
    n_cu: np.ndarray,
    s_iz: np.ndarray,
) -> ZapdosFields:
    """Cheap deterministic stand-in for Zapdos while file coupling is being wired."""

    geom = config.get("geometry", {})
    op = config.get("operation", {})
    ymax = float(geom.get("target_to_wafer", mesh.y_max))
    target_bias = float(op.get("target_bias", -500.0))
    plasma_guess = float(op.get("plasma_potential_guess", 10.0))
    r0 = float(geom.get("racetrack_radius", 0.105))
    rw = float(geom.get("racetrack_width", 0.025))

    x, y = mesh.centers_2d()

    # Two symmetric racetrack lobes at x = ±r0
    racetrack = (
        np.exp(-0.5 * ((x - r0) / max(rw, 1.0e-9)) ** 2)
        + np.exp(-0.5 * ((x + r0) / max(rw, 1.0e-9)) ** 2)
    ) * 0.5   # normalise so peak ≈ 1

    axial = np.exp(-y / max(0.20 * ymax, 1.0e-9))

    phi = (
        plasma_guess
        + (target_bias - plasma_guess)
        * np.exp(-y / max(0.035 * ymax, 1.0e-9))
        * (0.45 + 0.55 * racetrack)
    )
    phi += 6.0 * np.sin(np.pi * y / max(ymax, 1.0e-9)) * np.exp(-(x / max(mesh.x_max, 1.0e-9)) ** 2)

    dphi_dx, dphi_dy = gradient_2d(mesh, phi)
    ex = -dphi_dx
    ey = -dphi_dy

    s_norm = s_iz / max(float(np.max(s_iz)), 1.0)
    ne = 1.0e16 * (0.15 + 0.85 * racetrack * axial) + 3.0e15 * s_norm
    te = 2.0 + 3.5 * racetrack * axial
    return ZapdosFields(mesh=mesh, phi=phi, ex=ex, ey=ey, ne=ne, te=te, s_iz_zapdos=s_iz.copy())


def _grid_column(data: np.ndarray, column: str, mesh: CartesianMesh2D) -> np.ndarray:
    values = np.asarray(data[column], dtype=float)
    if values.size != mesh.nx * mesh.ny:
        raise ValueError(f"CSV field {column} has {values.size} values, expected {mesh.nx * mesh.ny}")
    order = np.lexsort((data["y"], data["x"]))
    return values[order].reshape(mesh.shape)
