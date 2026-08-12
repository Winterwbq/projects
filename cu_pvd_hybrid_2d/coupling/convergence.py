from __future__ import annotations

from dataclasses import dataclass


@dataclass
class ConvergenceState:
    converged: bool
    metrics: dict[str, float]


def relative_change(new: float, old: float) -> float:
    denom = max(abs(old), abs(new), 1.0e-30)
    return abs(new - old) / denom


def evaluate_convergence(history: list[dict[str, float]], tol: float) -> ConvergenceState:
    if len(history) < 2:
        return ConvergenceState(False, {})

    cur = history[-1]
    prev = history[-2]
    metrics = {
        "target_current_error": abs(cur.get("I_target_sim_A", 0.0) - cur.get("I_target_measured_A", 0.0))
        / max(abs(cur.get("I_target_measured_A", 0.0)), 1.0e-30),
        "wafer_cu_flux_change": relative_change(cur.get("cu_total_wafer_rate_s", 0.0), prev.get("cu_total_wafer_rate_s", 0.0)),
        "cu_plus_target_flux_change": relative_change(
            cur.get("cu_plus_target_rate_s", 0.0), prev.get("cu_plus_target_rate_s", 0.0)
        ),
        "total_ionization_change": relative_change(
            cur.get("total_ionization_rate_s", 0.0), prev.get("total_ionization_rate_s", 0.0)
        ),
        "source_loss_balance_error": abs(cur.get("source_loss_balance_error", 0.0)),
    }
    return ConvergenceState(all(v < tol for v in metrics.values()), metrics)

