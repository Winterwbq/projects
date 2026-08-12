from __future__ import annotations

import numpy as np


def boris_push(v: np.ndarray, efield: np.ndarray, bfield: np.ndarray, q_over_m: float, dt: float) -> np.ndarray:
    """Vectorized Boris velocity update.

    v, efield, bfield are shape (n, 3), ordered x,y,z in Cartesian components.
    """

    half = 0.5 * q_over_m * dt
    v_minus = v + half * efield
    t = half * bfield
    t_mag2 = np.sum(t * t, axis=1)[:, None]
    s = 2.0 * t / (1.0 + t_mag2)
    v_prime = v_minus + np.cross(v_minus, t)
    v_plus = v_minus + np.cross(v_prime, s)
    return v_plus + half * efield

