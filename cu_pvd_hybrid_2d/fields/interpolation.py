from __future__ import annotations

from dataclasses import dataclass
from typing import Iterable

import numpy as np


@dataclass(frozen=True)
class CartesianMesh2D:
    """Cell-centered 2D Cartesian mesh with finite-volume weights.

    x spans [-x_max, +x_max] (radial, symmetric about axis).
    y spans [0, y_max] (axial, target at y=0, wafer at y=y_max).

    All integrals are *per unit z-depth* (the ignorable third dimension),
    so cell_volumes returns [m²] and surface areas return [m].
    """

    x_edges: np.ndarray  # length nx+1
    y_edges: np.ndarray  # length ny+1

    @classmethod
    def from_bounds(
        cls,
        nx: int,
        ny: int,
        x_min: float,
        x_max: float,
        y_max: float,
    ) -> "CartesianMesh2D":
        return cls(
            x_edges=np.linspace(float(x_min), float(x_max), int(nx) + 1),
            y_edges=np.linspace(0.0, float(y_max), int(ny) + 1),
        )

    # ------------------------------------------------------------------
    # Cell-center coordinates
    # ------------------------------------------------------------------

    @property
    def x(self) -> np.ndarray:
        return 0.5 * (self.x_edges[:-1] + self.x_edges[1:])

    @property
    def y(self) -> np.ndarray:
        return 0.5 * (self.y_edges[:-1] + self.y_edges[1:])

    @property
    def nx(self) -> int:
        return self.x_edges.size - 1

    @property
    def ny(self) -> int:
        return self.y_edges.size - 1

    @property
    def shape(self) -> tuple[int, int]:
        return (self.nx, self.ny)

    @property
    def dx(self) -> float:
        return float(np.mean(np.diff(self.x_edges)))

    @property
    def dy(self) -> float:
        return float(np.mean(np.diff(self.y_edges)))

    @property
    def x_min(self) -> float:
        return float(self.x_edges[0])

    @property
    def x_max(self) -> float:
        return float(self.x_edges[-1])

    @property
    def y_max(self) -> float:
        return float(self.y_edges[-1])

    # ------------------------------------------------------------------
    # Geometric weights
    # ------------------------------------------------------------------

    def cell_volumes(self) -> np.ndarray:
        """Cell area per unit z-depth: dx_i * dy_j  [m²], shape (nx, ny)."""
        dx = np.diff(self.x_edges)   # (nx,)
        dy = np.diff(self.y_edges)   # (ny,)
        return dx[:, None] * dy[None, :]  # (nx, ny)

    def surface_areas_bottom(self) -> np.ndarray:
        """Cell widths along the bottom/top surface (target and wafer): dx_i [m], shape (nx,)."""
        return np.diff(self.x_edges)

    def surface_areas_side(self) -> np.ndarray:
        """Cell heights along the left/right side walls: dy_j [m], shape (ny,)."""
        return np.diff(self.y_edges)

    def centers_2d(self) -> tuple[np.ndarray, np.ndarray]:
        """Return (x2d, y2d) meshgrid arrays of shape (nx, ny)."""
        return np.meshgrid(self.x, self.y, indexing="ij")


# ---------------------------------------------------------------------------
# Named field container
# ---------------------------------------------------------------------------

@dataclass
class Field2D:
    mesh: CartesianMesh2D
    values: np.ndarray
    name: str = "field"
    units: str = ""

    def __post_init__(self) -> None:
        arr = np.asarray(self.values, dtype=float)
        if arr.shape != self.mesh.shape:
            raise ValueError(f"{self.name} shape {arr.shape} does not match mesh {self.mesh.shape}")
        self.values = arr

    def at(self, x: np.ndarray | float, y: np.ndarray | float) -> np.ndarray:
        return bilinear_on_centers(self.mesh.x, self.mesh.y, self.values, x, y)

    def integral(self) -> float:
        return integrate_2d(self.mesh, self.values)

    def copy(self, name: str | None = None) -> "Field2D":
        return Field2D(self.mesh, self.values.copy(), name=name or self.name, units=self.units)


# ---------------------------------------------------------------------------
# Interpolation
# ---------------------------------------------------------------------------

def bilinear_on_centers(
    x_grid: Iterable[float],
    y_grid: Iterable[float],
    values: np.ndarray,
    x_query: np.ndarray | float,
    y_query: np.ndarray | float,
) -> np.ndarray:
    """Bilinear interpolation on a regular cell-centered 2D Cartesian grid."""

    xg = np.asarray(x_grid, dtype=float)
    yg = np.asarray(y_grid, dtype=float)
    val = np.asarray(values, dtype=float)
    xq, yq = np.broadcast_arrays(np.asarray(x_query, dtype=float), np.asarray(y_query, dtype=float))

    xq_clip = np.clip(xq, xg[0], xg[-1])
    yq_clip = np.clip(yq, yg[0], yg[-1])

    ix = np.searchsorted(xg, xq_clip, side="right") - 1
    iy = np.searchsorted(yg, yq_clip, side="right") - 1
    ix = np.clip(ix, 0, xg.size - 2)
    iy = np.clip(iy, 0, yg.size - 2)

    x0 = xg[ix];  x1 = xg[ix + 1]
    y0 = yg[iy];  y1 = yg[iy + 1]
    tx = np.divide(xq_clip - x0, x1 - x0, out=np.zeros_like(xq_clip), where=(x1 != x0))
    ty = np.divide(yq_clip - y0, y1 - y0, out=np.zeros_like(yq_clip), where=(y1 != y0))

    f00 = val[ix, iy];      f10 = val[ix + 1, iy]
    f01 = val[ix, iy + 1];  f11 = val[ix + 1, iy + 1]
    return (1.0 - tx) * (1.0 - ty) * f00 + tx * (1.0 - ty) * f10 \
         + (1.0 - tx) * ty * f01       + tx * ty * f11


# ---------------------------------------------------------------------------
# Integration and utilities
# ---------------------------------------------------------------------------

def integrate_2d(mesh: CartesianMesh2D, field: np.ndarray) -> float:
    """Volume integral per unit z-depth: ∫∫ f dx dy [units of f × m²]."""
    arr = np.asarray(field, dtype=float)
    if arr.shape != mesh.shape:
        raise ValueError(f"field shape {arr.shape} does not match mesh {mesh.shape}")
    return float(np.sum(arr * mesh.cell_volumes()))


def normalize_volume_source(mesh: CartesianMesh2D, source: np.ndarray, total_rate: float) -> np.ndarray:
    src = np.maximum(np.asarray(source, dtype=float), 0.0)
    integral = integrate_2d(mesh, src)
    if integral <= 0.0:
        raise ValueError("cannot normalize a zero or negative source field")
    return src * (float(total_rate) / integral)


def smooth_conserve(mesh: CartesianMesh2D, field: np.ndarray, passes: int = 1) -> np.ndarray:
    """Conservative 5-point smoother for noisy MC tallies.

    Blends 50 % center weight with 50 % average-of-4-neighbours, then
    rescales to preserve the total integral over the domain.
    """
    old_total = integrate_2d(mesh, field)
    out = np.asarray(field, dtype=float).copy()
    for _ in range(max(0, int(passes))):
        padded = np.pad(out, ((1, 1), (1, 1)), mode="edge")
        out = (
            4.0 * padded[1:-1, 1:-1]
            + padded[:-2, 1:-1]
            + padded[2:, 1:-1]
            + padded[1:-1, :-2]
            + padded[1:-1, 2:]
        ) / 8.0
    new_total = integrate_2d(mesh, out)
    if new_total > 0.0 and old_total > 0.0:
        out *= old_total / new_total
    return out


def gradient_2d(mesh: CartesianMesh2D, scalar: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """Return (d/dx, d/dy) on the mesh cell centres using central differences."""
    arr = np.asarray(scalar, dtype=float)
    d_dx, d_dy = np.gradient(arr, mesh.x, mesh.y, edge_order=1)
    return d_dx, d_dy
