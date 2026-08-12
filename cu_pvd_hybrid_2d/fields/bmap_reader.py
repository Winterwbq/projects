from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

import numpy as np

from fields.interpolation import CartesianMesh2D, Field2D, bilinear_on_centers


@dataclass
class BMap:
    """2D Cartesian magnetic field map with in-plane Bx, By components only.

    Bz (out-of-plane) is always zero in the 2D XY model.  The ``at`` method
    returns a (n, 3) Cartesian array [Bx, By, 0] so that the Boris pusher
    can handle the 2.5D velocity space (vx, vy, vz) directly.
    """

    mesh: CartesianMesh2D
    bx: np.ndarray   # shape (nx, ny)
    by: np.ndarray   # shape (nx, ny)
    name: str

    def __post_init__(self) -> None:
        for label, arr in (("bx", self.bx), ("by", self.by)):
            if np.asarray(arr).shape != self.mesh.shape:
                raise ValueError(f"{self.name}.{label} shape {np.asarray(arr).shape} "
                                 f"does not match mesh {self.mesh.shape}")

    def at(self, x: np.ndarray | float, y: np.ndarray | float) -> np.ndarray:
        """Return field at query points as (n, 3) Cartesian array [Bx, By, 0]."""
        bx = bilinear_on_centers(self.mesh.x, self.mesh.y, self.bx, x, y)
        by = bilinear_on_centers(self.mesh.x, self.mesh.y, self.by, x, y)
        zeros = np.zeros_like(bx)
        return np.stack([bx, by, zeros], axis=-1)

    def magnitude(self) -> Field2D:
        return Field2D(self.mesh, np.sqrt(self.bx**2 + self.by**2), name=f"|{self.name}|", units="T")

    def plus(self, other: "BMap", name: str = "B_total") -> "BMap":
        if self.mesh != other.mesh:
            other = other.resample(self.mesh)
        return BMap(
            mesh=self.mesh,
            bx=self.bx + other.bx,
            by=self.by + other.by,
            name=name,
        )

    def resample(self, mesh: CartesianMesh2D) -> "BMap":
        x2d, y2d = mesh.centers_2d()
        return BMap(
            mesh=mesh,
            bx=bilinear_on_centers(self.mesh.x, self.mesh.y, self.bx, x2d, y2d),
            by=bilinear_on_centers(self.mesh.x, self.mesh.y, self.by, x2d, y2d),
            name=self.name,
        )


def read_bmap_csv(path: str | Path, mesh: CartesianMesh2D | None = None, name: str = "B_map") -> BMap:
    """Read a CSV with columns x,y,Bx,By."""

    data = np.genfromtxt(Path(path), delimiter=",", names=True)
    x_unique = np.unique(data["x"])
    y_unique = np.unique(data["y"])
    src_mesh = CartesianMesh2D(
        x_edges=_centers_to_edges(x_unique),
        y_edges=_centers_to_edges(y_unique, lower=0.0),
    )
    shape = src_mesh.shape

    order = np.lexsort((data["y"], data["x"]))
    rows = data[order]
    bx = rows["Bx"].reshape(shape)
    by = rows["By"].reshape(shape)

    bmap = BMap(src_mesh, bx=bx, by=by, name=name)
    return bmap.resample(mesh) if mesh is not None else bmap


def load_bmaps(config: dict[str, Any], mesh: CartesianMesh2D) -> dict[str, BMap]:
    bcfg = config.get("bfield", {})
    magnetron_path = bcfg.get("magnetron_csv")
    external_path = bcfg.get("external_csv")

    magnetron = (
        read_bmap_csv(magnetron_path, mesh=mesh, name="B_magnetron")
        if magnetron_path
        else synthetic_magnetron_bmap(mesh, config)
    )
    external = (
        read_bmap_csv(external_path, mesh=mesh, name="B_external")
        if external_path
        else synthetic_external_bmap(mesh, config)
    )
    total = magnetron.plus(external, name="B_total")
    return {"B_magnetron": magnetron, "B_external": external, "B_total": total}


def synthetic_magnetron_bmap(mesh: CartesianMesh2D, config: dict[str, Any]) -> BMap:
    """Synthetic magnetron field map.

    The default is one instantaneous rolling-magnet lobe at x = +r0. Set
    geometry.racetrack_mode: symmetric to use mirror-image lobes at x = +/-r0.
    Each lobe contributes a poloidal (in-plane) field pattern; Bz is zero.
    """
    geom = config.get("geometry", {})
    bcfg = config.get("bfield", {})
    r0 = float(geom.get("racetrack_radius", 0.105))
    width = float(geom.get("racetrack_width", 0.025))
    decay = float(bcfg.get("decay_length", 0.10))
    b0 = float(bcfg.get("synthetic_magnetron_B0_T", 0.055))
    mode = str(geom.get("racetrack_mode", "one_sided")).lower()

    x, y = mesh.centers_2d()

    # Gaussian envelope centred at the instantaneous racetrack location.
    env_pos = np.exp(-0.5 * ((x - r0) / max(width, 1.0e-9)) ** 2) * np.exp(-y / max(decay, 1.0e-9))

    # Bx: radial-like component (points away from lobe centre in x)
    bx_pos = -b0 * (y / max(decay, 1.0e-9)) * env_pos
    bx = bx_pos

    # By: axial-like component (peaks at lobe, decays axially)
    by = b0 * env_pos
    if mode in {"symmetric", "two_sided", "two-sided", "mirror"}:
        env_neg = np.exp(-0.5 * ((x + r0) / max(width, 1.0e-9)) ** 2) * np.exp(
            -y / max(decay, 1.0e-9)
        )
        bx = bx + b0 * (y / max(decay, 1.0e-9)) * env_neg
        by = by + b0 * env_neg

    return BMap(mesh=mesh, bx=bx, by=by, name="B_magnetron")


def synthetic_external_bmap(mesh: CartesianMesh2D, config: dict[str, Any]) -> BMap:
    """Uniform axial (y-direction) external field."""
    b0 = float(config.get("bfield", {}).get("synthetic_external_By_T", 0.0))
    zeros = np.zeros(mesh.shape)
    by = np.full(mesh.shape, b0)
    return BMap(mesh=mesh, bx=zeros.copy(), by=by, name="B_external")


def _centers_to_edges(centers: np.ndarray, lower: float | None = None) -> np.ndarray:
    centers = np.asarray(centers, dtype=float)
    if centers.size == 1:
        dx = max(abs(centers[0]), 1.0)
        return np.array([centers[0] - 0.5 * dx, centers[0] + 0.5 * dx])
    mids = 0.5 * (centers[:-1] + centers[1:])
    first = centers[0] - (mids[0] - centers[0])
    last = centers[-1] + (centers[-1] - mids[-1])
    if lower is not None:
        first = lower
    return np.concatenate([[first], mids, [last]])
