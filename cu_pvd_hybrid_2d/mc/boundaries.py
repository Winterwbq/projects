from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ChamberGeometry:
    """2D Cartesian chamber geometry.

    x ∈ [-chamber_radius, +chamber_radius] (symmetric about the axis).
    y ∈ [0, target_to_wafer] (target at y=0, wafer at y=target_to_wafer).

    Target and wafer are identified by |x| < their respective radii;
    the remainder of the bottom/top boundary is a grounded shield/wall.
    The side walls are at x = ±chamber_radius.
    """

    target_to_wafer: float
    target_radius: float
    wafer_radius: float
    chamber_radius: float

    @classmethod
    def from_config(cls, config: dict) -> "ChamberGeometry":
        geom = config.get("geometry", {})
        return cls(
            target_to_wafer=float(geom.get("target_to_wafer", 0.60)),
            target_radius=float(geom.get("target_radius", 0.18)),
            wafer_radius=float(geom.get("wafer_radius", 0.15)),
            chamber_radius=float(geom.get("chamber_radius", 0.24)),
        )

    def classify(self, x: float, y: float) -> str | None:
        """Return the boundary hit at (x, y), or None if inside the domain.

        Returns one of "target", "wafer", "wall", or None.
        """
        ax = abs(x)
        if y <= 0.0 and ax <= self.target_radius:
            return "target"
        if y >= self.target_to_wafer and ax <= self.wafer_radius:
            return "wafer"
        # Bottom shield (y=0, outside target) and top shield (y=L, outside wafer)
        # as well as the left/right side walls.
        if ax >= self.chamber_radius or y <= 0.0 or y >= self.target_to_wafer:
            return "wall"
        return None
