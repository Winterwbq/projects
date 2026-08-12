#!/usr/bin/env python3
"""Create a clear, slide-ready workflow for the Cartesian X-Z Zapdos model."""

from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch


ROOT = Path(__file__).resolve().parents[1]
PNG_OUTPUT = ROOT / "post" / "cartesian_xz_solver_workflow_simplified.png"
SVG_OUTPUT = ROOT / "post" / "cartesian_xz_solver_workflow_simplified.svg"

BG = "#F7FAFC"
INK = "#17263A"
MUTED = "#53687B"
BLUE = "#176B87"
TEAL = "#168A80"
ORANGE = "#C88018"
RED = "#BD4C4C"
LINE = "#AFC2CE"
SETUP_FILL = "#E8F4F3"
PHYSICS_FILL = "#EAF2F8"
SOLVE_FILL = "#ECEEFA"
FEEDBACK_FILL = "#FFF2DB"


def rounded_box(ax, x, y, w, h, fill, edge, radius=0.10, lw=1.5):
    patch = FancyBboxPatch(
        (x, y),
        w,
        h,
        boxstyle=f"round,pad=0.03,rounding_size={radius}",
        linewidth=lw,
        edgecolor=edge,
        facecolor=fill,
        zorder=2,
    )
    ax.add_patch(patch)
    return patch


def arrow(ax, start, end, color=BLUE, width=2.0, style="-", rad=0.0):
    patch = FancyArrowPatch(
        start,
        end,
        arrowstyle="-|>",
        mutation_scale=16,
        linewidth=width,
        linestyle=style,
        color=color,
        connectionstyle=f"arc3,rad={rad}",
        shrinkA=2,
        shrinkB=2,
        zorder=3,
    )
    ax.add_patch(patch)


def setup_card(ax, x, number, title, lines):
    y, w, h = 5.93, 4.82, 1.13
    rounded_box(ax, x, y, w, h, SETUP_FILL, TEAL)
    ax.text(
        x + 0.28,
        y + h / 2,
        number,
        ha="center",
        va="center",
        fontsize=15,
        fontweight="bold",
        color="white",
        bbox=dict(boxstyle="circle,pad=0.30", facecolor=TEAL, edgecolor="none"),
        zorder=4,
    )
    ax.text(x + 0.72, y + 0.82, title, ha="left", va="center", fontsize=12.4,
            fontweight="bold", color=INK, zorder=4)
    ax.text(x + 0.72, y + 0.39, lines, ha="left", va="center", fontsize=9.35,
            color=INK, linespacing=1.25, zorder=4)


def timestep_card(ax, x, number, title, lines, fill, edge, w=2.38):
    y, h = 2.45, 2.46
    rounded_box(ax, x, y, w, h, fill, edge)
    ax.text(
        x + 0.28,
        y + h - 0.31,
        number,
        ha="center",
        va="center",
        fontsize=11.5,
        fontweight="bold",
        color="white",
        bbox=dict(boxstyle="circle,pad=0.26", facecolor=edge, edgecolor="none"),
        zorder=4,
    )
    ax.text(x + 0.60, y + h - 0.32, title, ha="left", va="center",
            fontsize=10.4, fontweight="bold", color=INK, linespacing=0.95, zorder=4)
    for i, line in enumerate(lines):
        yy = y + h - 0.82 - i * 0.42
        ax.text(x + 0.22, yy, "•", ha="left", va="center", fontsize=11.5,
                color=edge, fontweight="bold", zorder=4)
        ax.text(x + 0.40, yy, line, ha="left", va="center", fontsize=8.55,
                color=INK, zorder=4)


def build_figure():
    plt.rcParams.update({"font.family": "Arial", "mathtext.fontset": "stixsans"})
    fig, ax = plt.subplots(figsize=(16, 9), dpi=140)
    fig.patch.set_facecolor(BG)
    ax.set_facecolor(BG)
    ax.set_xlim(0, 16)
    ax.set_ylim(0, 9)
    ax.axis("off")

    ax.text(0.55, 8.38, "How One Cartesian X–Z Zapdos Time Step Is Solved",
            ha="left", va="center", color=INK, fontsize=27, fontweight="bold")
    ax.text(0.58, 7.93,
            "Static model definition → coupled spatial–temporal residual → nonlinear solve → SEE feedback",
            ha="left", va="center", color=MUTED, fontsize=13.5)

    ax.text(0.55, 7.46, "DEFINED ONCE BEFORE TIME STEPPING", ha="left", va="center",
            fontsize=10.8, fontweight="bold", color=TEAL)
    ax.plot([3.52, 15.45], [7.46, 7.46], color=LINE, linewidth=1.1)

    setup_card(
        ax, 0.55, "A", "Geometry and finite-element mesh",
        "25 × 30 cm chamber; 48,000 QUAD4 elements\nQ1 nodal fields; refined near target and wafer",
    )
    setup_card(
        ax, 5.55, "B", "Initial state and prescribed maps",
        r"$U^0=[\ln n_e,\ln n_{Cu^+},\ln\bar\varepsilon_e,\phi]$" + "\n" +
        r"neutral gas, $B_x/B_z$, and normalized SEE profile",
    )
    setup_card(
        ax, 10.55, "C", "Boundary conditions and controls",
        "Powered target, grounded wafer, equal side-wall loss\nflux BCs, voltage waveform, SEE cap and filter",
    )
    arrow(ax, (5.40, 6.49), (5.53, 6.49), color=TEAL, width=1.8)
    arrow(ax, (10.40, 6.49), (10.53, 6.49), color=TEAL, width=1.8)

    ax.text(0.55, 5.45, r"FOR EVERY STEP  $t^n \rightarrow t^{n+1}=t^n+\Delta t$",
            ha="left", va="center", fontsize=11.2, fontweight="bold", color=BLUE)
    ax.text(15.40, 5.45,
            "ONE coupled residual:  Galerkin = space  •  implicit Euler = time  •  Newton = solve",
            ha="right", va="center", fontsize=9.7, fontweight="bold", color=ORANGE)

    xs = [0.42, 3.02, 5.62, 8.22, 10.82, 13.42]
    timestep_card(
        ax, xs[0], "1", "Start trial\nstate",
        [r"known state $U^n$", r"choose $\Delta t$", r"guess $U^{n+1,(0)}\approx U^n$"],
        SETUP_FILL, TEAL,
    )
    timestep_card(
        ax, xs[1], "2", "Evaluate local\nphysics",
        ["Q1 interpolation", r"$\mathbf{E}=-\nabla\phi$", "mobility, diffusion, reactions"],
        PHYSICS_FILL, BLUE,
    )
    timestep_card(
        ax, xs[2], "3", "Apply magnetic\ntransport",
        [r"split $\Gamma_e$ into $\parallel$ and $\perp$", r"suppress $\perp$: $/(1+\mu_e^2B^2)$", r"$Cu^+$ is unmagnetized"],
        PHYSICS_FILL, BLUE,
    )
    timestep_card(
        ax, xs[3], "4", "Assemble coupled\nresidual",
        ["Galerkin: spatial weak form", "implicit Euler: transient term", "couple all PDEs and BCs"],
        SOLVE_FILL, BLUE,
    )
    timestep_card(
        ax, xs[4], "5", "Newton nonlinear\nsolve",
        [r"evaluate $J$ and $R$", r"LU: $J\,\delta U=-R$", r"line search update $U+\lambda\delta U$"],
        SOLVE_FILL, BLUE,
    )
    timestep_card(
        ax, xs[5], "6", "Accept step &\nSEE feedback",
        [r"store converged $U^{n+1}$", "integrate target ion flux", "filter SEE; output; adapt Δt"],
        FEEDBACK_FILL, ORANGE,
    )

    for left, right in zip(xs[:-1], xs[1:]):
        arrow(ax, (left + 2.40, 3.67), (right - 0.03, 3.67), color=BLUE, width=2.0)

    # Newton iterations re-evaluate physics and rebuild the coupled residual.
    arrow(ax, (11.95, 2.39), (3.98, 2.39), color=BLUE, width=1.8, style="--", rad=-0.22)
    ax.text(7.95, 1.43,
            "Newton iteration: update the trial fields, then repeat steps 2–5 until the residual is small",
            ha="center", va="center", fontsize=10.5, color=BLUE, fontweight="bold",
            bbox=dict(facecolor=BG, edgecolor="none", pad=1.5))

    # Failed nonlinear solve: retry the same physical interval with a smaller step.
    arrow(ax, (12.02, 2.31), (1.62, 2.31), color=RED, width=1.55, style=":", rad=-0.30)
    ax.text(6.95, 0.91, "If convergence fails: reduce Δt and retry from the last accepted state",
            ha="center", va="center", fontsize=10.0, color=RED,
            bbox=dict(facecolor=BG, edgecolor="none", pad=1.5))

    # Accepted-step feedback to the next step.
    arrow(ax, (14.78, 2.36), (1.18, 2.36), color=ORANGE, width=2.2, rad=-0.43)
    ax.text(8.12, 0.43,
            r"Next step: $U^{n+1}$ becomes the known state and the filtered target flux sets the new SEE amplitude",
            ha="center", va="center", fontsize=10.7, color=ORANGE, fontweight="bold",
            bbox=dict(facecolor=BG, edgecolor="none", pad=1.5))

    PNG_OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(PNG_OUTPUT, dpi=140, facecolor=BG, edgecolor="none", bbox_inches="tight", pad_inches=0.08)
    fig.savefig(SVG_OUTPUT, facecolor=BG, edgecolor="none", bbox_inches="tight", pad_inches=0.08)
    plt.close(fig)
    return PNG_OUTPUT, SVG_OUTPUT


if __name__ == "__main__":
    png, svg = build_figure()
    print(png)
    print(svg)
