from __future__ import annotations

import numpy as np

from fields.interpolation import CartesianMesh2D, smooth_conserve


def under_relax(old: np.ndarray, new: np.ndarray, alpha: float) -> np.ndarray:
    a = float(alpha)
    if not 0.0 <= a <= 1.0:
        raise ValueError("relaxation alpha must be in [0, 1]")
    return (1.0 - a) * np.asarray(old, dtype=float) + a * np.asarray(new, dtype=float)


def smooth_then_relax(
    mesh: CartesianMesh2D,
    old: np.ndarray,
    new: np.ndarray,
    alpha: float,
    smoothing_passes: int = 1,
) -> np.ndarray:
    smoothed = smooth_conserve(mesh, new, passes=smoothing_passes)
    return under_relax(old, smoothed, alpha)
