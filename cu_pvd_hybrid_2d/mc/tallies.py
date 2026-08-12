from __future__ import annotations

from dataclasses import dataclass, field

import numpy as np

from fields.interpolation import CartesianMesh2D


@dataclass
class SurfaceFluxTally:
    """Accumulates surface flux tallies on the 2D Cartesian mesh.

    ``target_flux_x`` and ``wafer_flux_x`` are arrays of shape ``(nx,)``
    indexed by the x-cell index.  Values are in m⁻¹ s⁻¹ (flux per unit
    x-width, per unit z-depth) so they integrate to a total rate via:

        rate = sum(flux_x * mesh.surface_areas_bottom())
    """

    mesh: CartesianMesh2D
    target_flux_x: np.ndarray = field(init=False)
    wafer_flux_x: np.ndarray = field(init=False)
    wall_loss_rate: float = 0.0
    target_rate: float = 0.0
    wafer_rate: float = 0.0

    def __post_init__(self) -> None:
        self.target_flux_x = np.zeros(self.mesh.nx)
        self.wafer_flux_x = np.zeros(self.mesh.nx)

    def add_target(self, x: float, rate: float) -> None:
        i = _x_index(self.mesh, x)
        if i is not None:
            self.target_flux_x[i] += rate / self.mesh.surface_areas_bottom()[i]
            self.target_rate += rate

    def add_wafer(self, x: float, rate: float) -> None:
        i = _x_index(self.mesh, x)
        if i is not None:
            self.wafer_flux_x[i] += rate / self.mesh.surface_areas_bottom()[i]
            self.wafer_rate += rate

    def add_wall(self, rate: float) -> None:
        self.wall_loss_rate += rate


@dataclass
class NeutralMCResult:
    n_cu: np.ndarray           # shape (nx, ny), m⁻³ per unit z-depth
    gamma_wafer: np.ndarray    # shape (nx,), m⁻¹ s⁻¹
    gamma_wall_rate: float     # s⁻¹
    gamma_target_return: np.ndarray  # shape (nx,), m⁻¹ s⁻¹
    total_source_rate: float   # s⁻¹


@dataclass
class IonMCResult:
    gamma_target: np.ndarray   # shape (nx,), m⁻¹ s⁻¹
    gamma_wafer: np.ndarray    # shape (nx,), m⁻¹ s⁻¹
    gamma_wall_rate: float     # s⁻¹
    iedf_wafer_eV: np.ndarray
    angle_wafer_deg: np.ndarray
    sheath_edge_energy_angle: np.ndarray
    total_birth_rate: float    # s⁻¹


def _x_index(mesh: CartesianMesh2D, x: float) -> int | None:
    if x < mesh.x_edges[0] or x >= mesh.x_edges[-1]:
        return None
    return int(np.clip(np.searchsorted(mesh.x_edges, x, side="right") - 1, 0, mesh.nx - 1))
