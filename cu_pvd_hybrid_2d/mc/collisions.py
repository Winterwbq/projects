from __future__ import annotations


class NullCollisionModel:
    """Placeholder for Ar/Cu collisions.

    First version intentionally keeps neutral Cu ballistic. The interface is here
    so DSMC or null-collision scattering can be added without changing the MC
    drivers.
    """

    def scatter_neutral(self, *args, **kwargs) -> None:
        return None

    def scatter_ion(self, *args, **kwargs) -> None:
        return None

