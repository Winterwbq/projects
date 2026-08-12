from __future__ import annotations

import csv
from pathlib import Path

import numpy as np

from fields.interpolation import CartesianMesh2D


def fake_magnetron_components(
    mesh: CartesianMesh2D,
    *,
    racetrack_radius: float = 0.105,
    racetrack_width: float = 0.040,
    decay_length: float = 0.090,
    peak_field_t: float = 0.100,
    background_by_t: float = -0.0015,
) -> tuple[np.ndarray, np.ndarray]:
    """Return smooth 2D Cartesian fake magnetron field components (Bx, By).

    The field is a controlled test map shaped like one instantaneous
    rolling-magnet position in the XY plane. It uses two virtual magnetic poles
    below the target to create local loop-like field lines that leave and
    return to the target, with no out-of-plane (Bz) component.
    """

    x, y = mesh.centers_2d()
    axial = y / max(decay_length, 1.0e-12)
    width = max(racetrack_width, 1.0e-12)

    pole_sep = max(2.4 * width, 0.055)
    pole_y = -0.022
    softening = 0.012
    left_pole_x = racetrack_radius - 0.5 * pole_sep
    right_pole_x = racetrack_radius + 0.5 * pole_sep

    def pole_field(pole_x: float, pole_charge: float) -> tuple[np.ndarray, np.ndarray]:
        dx = x - pole_x
        dy = y - pole_y
        r2 = dx**2 + dy**2 + softening**2
        return pole_charge * dx / r2, pole_charge * dy / r2

    bx_left, by_left = pole_field(left_pole_x, 1.0)
    bx_right, by_right = pole_field(right_pole_x, -1.0)
    bx = bx_left + bx_right
    by = by_left + by_right

    # Keep this as one instantaneous rolling-magnet spot instead of a full
    # symmetric racetrack by tapering the far-field contribution smoothly.
    lateral_envelope = np.exp(-0.5 * ((x - racetrack_radius) / (2.5 * pole_sep)) ** 2)
    axial_envelope = np.exp(-0.45 * axial)
    bx *= lateral_envelope * axial_envelope
    by *= lateral_envelope * axial_envelope

    max_b = max(float(np.max(np.sqrt(bx**2 + by**2))), 1.0e-30)
    scale = peak_field_t / max_b
    bx *= scale
    by = by * scale + background_by_t

    return bx, by


def write_fake_magnetron_bmap(
    path: str | Path,
    mesh: CartesianMesh2D,
    *,
    racetrack_radius: float = 0.105,
    racetrack_width: float = 0.040,
    decay_length: float = 0.090,
    peak_field_t: float = 0.100,
    background_by_t: float = -0.0015,
) -> None:
    """Write fake magnetron B-map to CSV with columns x, y, Bx, By."""
    bx, by = fake_magnetron_components(
        mesh,
        racetrack_radius=racetrack_radius,
        racetrack_width=racetrack_width,
        decay_length=decay_length,
        peak_field_t=peak_field_t,
        background_by_t=background_by_t,
    )
    out = Path(path)
    out.parent.mkdir(parents=True, exist_ok=True)

    with out.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["x", "y", "Bx", "By"])
        for ix, xv in enumerate(mesh.x):
            for iy, yv in enumerate(mesh.y):
                writer.writerow([xv, yv, bx[ix, iy], by[ix, iy]])
