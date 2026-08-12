from __future__ import annotations

from dataclasses import dataclass

import numpy as np

ELEMENTARY_CHARGE = 1.602176634e-19
AMU = 1.66053906660e-27
EV_TO_J = ELEMENTARY_CHARGE
CU_MASS_KG = 63.546 * AMU


@dataclass
class ParticleBatch:
    x: np.ndarray
    y: np.ndarray
    z: np.ndarray
    vx: np.ndarray
    vy: np.ndarray
    vz: np.ndarray
    weight: np.ndarray
    alive: np.ndarray

    @classmethod
    def empty(cls, n: int) -> "ParticleBatch":
        zeros = np.zeros(n, dtype=float)
        return cls(
            x=zeros.copy(),
            y=zeros.copy(),
            z=zeros.copy(),
            vx=zeros.copy(),
            vy=zeros.copy(),
            vz=zeros.copy(),
            weight=zeros.copy(),
            alive=np.ones(n, dtype=bool),
        )

    @property
    def r(self) -> np.ndarray:
        return np.sqrt(self.x**2 + self.y**2)

    @property
    def speed(self) -> np.ndarray:
        return np.sqrt(self.vx**2 + self.vy**2 + self.vz**2)


def speed_from_ev(energy_ev: np.ndarray | float, mass_kg: float = CU_MASS_KG) -> np.ndarray:
    return np.sqrt(2.0 * np.asarray(energy_ev, dtype=float) * EV_TO_J / mass_kg)


def sample_maxwellian_velocity(rng: np.random.Generator, n: int, temperature_ev: float, mass_kg: float = CU_MASS_KG) -> np.ndarray:
    sigma = np.sqrt(float(temperature_ev) * EV_TO_J / mass_kg)
    return rng.normal(0.0, sigma, size=(n, 3))

