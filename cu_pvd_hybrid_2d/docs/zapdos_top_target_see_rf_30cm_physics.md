# Zapdos Top-Target SEE RF 30 cm Input: Equations, Coefficients, And Sources

This note summarizes the physics implemented by
`zapdos_templates/cu_pvd_hybrid_top_target_see_rf_30cm.i`, using the input file
and the corresponding Zapdos/Crane source code.

## Model Scope

The input is a reduced Cu PVD plasma model in 2D axisymmetric R-Z geometry.

- `x = r`, radial coordinate, from `0` to `0.25 m`
- `y = z`, axial coordinate, from wafer at `z = 0` to target at `z = 0.30 m`
- The left boundary is the symmetry axis.
- The right boundary is the plasma-side edge of an unresolved sheath adjacent
  to an external grounded metal wall.
- The bottom boundary is the RF-biased wafer plus bottom shield.
- The top boundary is the powered target.

The active plasma species are electrons and `Cu+`. Neutral Cu is prescribed as
an auxiliary background field. Argon ionization and Ar+ transport are disabled.

The solved nonlinear variables are logarithmic molar densities:

$$
\begin{aligned}
\mathrm{em} &= \ln\!\left(\frac{n_e}{N_A}\right), \\
\mathrm{Cu^+} &= \ln\!\left(\frac{n_{\mathrm{Cu}^+}}{N_A}\right), \\
\mathrm{mean\_en} &= \ln\!\left(\frac{n_e\epsilon}{N_A}\right), \\
\mathrm{potential} &= \phi .
\end{aligned}
$$

where `n_e` and `n_Cu+` are particle densities, `N_A` is Avogadro's number, and
`epsilon` is the electron mean energy in eV.

`em` contains only the bulk electron population. The unresolved fast
secondary-electron beam is not inserted through a particle or energy boundary
condition; its pair production and retained energy are represented by volume
sources.

## Governing Equations

Zapdos `DriftDiffusionAction` supplies the drift-diffusion equations for
electrons, ions, electron mean energy, and electrostatic potential.

For a charged species `j`,

$$
\frac{\partial n_j}{\partial t}
+ \nabla \cdot \boldsymbol{\Gamma}_j
= S_j
$$

$$
\boldsymbol{\Gamma}_j
= s_j\mu_j\mathbf{E}n_j - D_j\nabla n_j
$$

where:

- `s_j = -1` for electrons and `s_j = +1` for positive ions.
- `mu_j` is mobility.
- `D_j` is diffusion coefficient.
- $\mathbf{E} = -\nabla\phi$ is electric field.
- `S_j` is the volumetric source term.

For this input:

$$
\boldsymbol{\Gamma}_e
= -\mu_e\mathbf{E}n_e - D_e\nabla n_e
$$

$$
\boldsymbol{\Gamma}_{\mathrm{Cu}^+}
= \mu_{\mathrm{Cu}^+}\mathbf{E}n_{\mathrm{Cu}^+}
- D_{\mathrm{Cu}^+}\nabla n_{\mathrm{Cu}^+}
$$

Electron and electron-energy transport use the prescribed magnetized transport
operator from `Bx_total_func` and `By_total_func`. For an unmagnetized in-plane
flux vector $\mathbf a$, the code forms

$$
\begin{aligned}
\widehat{\mathbf b} &= \frac{\mathbf B}{|\mathbf B|}, \\
\mathbf a_{\parallel} &= (\mathbf a\cdot\widehat{\mathbf b})\widehat{\mathbf b}, \\
\mathbf a_{\perp} &= \mathbf a-\mathbf a_{\parallel}, \\
\beta_{\mathrm{eff}}^2 &= \min\!\left(\mu^2|\mathbf B|^2,10^4\right), \\
\mathcal M_B(\mathbf a)
&=\mathbf a_{\parallel}
+\frac{\mathbf a_{\perp}}{1+\beta_{\mathrm{eff}}^2}.
\end{aligned}
$$

Thus the active electron flux is

$$
\boldsymbol{\Gamma}_e
=\mathcal M_B\!\left(
-\mu_e\mathbf E n_e-D_e\nabla n_e
\right).
$$

Parallel transport is unchanged, while cross-field transport is reduced by
$1/(1+\beta_{\mathrm{eff}}^2)$. The cap corresponds to
$|\beta_{\mathrm{eff}}|\le100$ and prevents a numerically singular
cross-field operator. There is no in-plane Hall term: for an R-Z magnetic field
the Hall drift is azimuthal, outside this 2D model. Cu+ transport is not
magnetized.

The electron mean-energy density is:

$$
n_{\epsilon} = n_e\epsilon
$$

and Zapdos solves:

$$
\frac{\partial n_{\epsilon}}{\partial t}
+ \nabla \cdot
\left(
-\mu_{\epsilon}\mathbf{E}n_{\epsilon}
-D_{\epsilon}\nabla n_{\epsilon}
\right)
- \boldsymbol{\Gamma}_e \cdot \mathbf{E}
= S_{\epsilon}
$$

The Joule term `-Gamma_e dot E` couples electron transport to the mean-energy
equation. Zapdos uses:

$$
\mu_{\epsilon} = \frac{5}{3}\mu_e,
\qquad
D_{\epsilon} = \frac{5}{3}D_e,
\qquad
T_e = \frac{2}{3}\epsilon .
$$

The electrostatic potential is governed by Poisson's equation:

$$
\nabla \cdot \left(-\epsilon_0\nabla\phi\right)
= e\left(n_{\mathrm{Cu}^+}-n_e\right)
$$

because the only charged heavy species is `Cu+`.

## Electron-Impact Cu Ionization

The `[Reactions/Copper]` block defines:

$$
e + \mathrm{Cu}
\rightarrow
e + e + \mathrm{Cu}^+
$$

with an EEDF rate table named `reaction1`.

In physical form:

$$
\begin{aligned}
S_{e,\mathrm{iz}}
&= k_{\mathrm{iz}}(\epsilon)n_en_{\mathrm{Cu}}, \\
S_{\mathrm{Cu}^+,\mathrm{iz}}
&= k_{\mathrm{iz}}(\epsilon)n_en_{\mathrm{Cu}}, \\
S_{\epsilon,\mathrm{iz}}
&= -E_{\mathrm{iz}}k_{\mathrm{iz}}(\epsilon)n_en_{\mathrm{Cu}} .
\end{aligned}
$$

where:

- `k_iz(epsilon)` is the Cu electron-impact ionization rate coefficient.
- `E_iz = 7.73 eV` is the Cu ionization threshold.
- `n_Cu` is the prescribed neutral Cu density.

The rate table is stored at:

```text
rate_coefficients_cu/reaction1.txt
```

The local README says this table was generated from BOLSIG Cu data. Since this
input uses `use_moles = true`, `reaction1.txt` stores `k_iz * N_A` in
`m^3 mol^-1 s^-1`, rather than the particle-unit rate `m^3 particle^-1 s^-1`.

## Current Target-Ion-Flux-Driven SEE Feedback

### Short Answer

The active target SEE source is **not prescribed**. It is driven by the
calculated Cu+ Bohm flux reaching the target:

$$
\boxed{
S_{\mathrm{SEE}}
\mathrel{\propto}
\gamma\,
\overline{\Gamma}_{B,\mathrm{target}}^{\,\mathrm{filtered}}\,
Y_{\mathrm{iz}}\,
W(r,z)
}
$$

The complete reduced feedback loop is:

$$
\mathbf B
\longrightarrow
\left(n_e,T_e,n_{\mathrm{Cu}^+},\text{transport and losses}\right)
\longrightarrow
\Gamma_{B,\mathrm{target}}
\longrightarrow
\text{power cap and response filter}
\longrightarrow
S_{\mathrm{SEE}}
\longrightarrow
\left(n_e,T_e,n_{\mathrm{Cu}^+}\right).
$$

The magnetic field does not directly change the prescribed values of
$\gamma$, $\eta_{\mathrm{iz}}$, $V_{\mathrm{sheath}}$, or the source
deposition window. It changes the target SEE source indirectly through the
plasma solution and calculated target Cu+ flux.

The phrase "target SEE source" does not mean that plasma is created exactly on
the target boundary. The target remains an absorbing boundary. The code
measures ions reaching it and deposits the reduced consequences of secondary
emission as volume sources below the target.

### 1. Raw Target Cu+ Bohm Flux

The logarithmic plasma variables give:

$$
\begin{aligned}
n_e &= N_A\exp(\mathrm{em}),\\
n_{\mathrm{Cu}^+} &= N_A\exp(\mathrm{Cu^+}),\\
\bar{\epsilon}_e
&=\exp(\mathrm{mean\_en}-\mathrm{em})\quad[\mathrm{eV}],\\
T_e&=\frac{2}{3}\bar{\epsilon}_e\quad[\mathrm{eV}].
\end{aligned}
$$

Internally, because `use_moles = true`, the target collector evaluates the
Bohm flux using the molar Cu+ density. In particle-unit notation, the local
sound speed and particle flux are:

$$
c_s(r,t)=
\sqrt{
\frac{eT_e(r,t)+k_BT_i}{m_{\mathrm{Cu}^+}}
},
$$

$$
\Gamma_B(r,t)=n_{\mathrm{Cu}^+}(r,t)c_s(r,t).
$$

The active input selects:

```text
flux_model = bohm
use_moles = true
```

Therefore, the code-level flux has units:

$$
[\Gamma_B]=\mathrm{mol\,m^{-2}\,s^{-1}}.
$$

Only the nonnegative incoming flux is retained. The scalar used by the
feedback model is the target-area average:

$$
\overline{\Gamma}_B(t)
=
\frac{1}{A_t}
\int_{A_t}\Gamma_B(r,t)\,dA.
$$

The model does calculate and retain a radial diagnostic profile, but the
active lagged response source does not use that local profile. It uses
$\overline{\Gamma}_B$ and subsequently imposes a prescribed annular source
shape.

### 2. Target Current and Downward-Only Power Cap

The calculated Cu+ ion current is:

$$
I_i
=
F\int_{A_t}\Gamma_B\,dA,
$$

where:

$$
F=eN_A
$$

is the Faraday constant.

The code's `target_discharge_current` diagnostic includes the incoming ion
current and the magnitude of the secondary-electron current:

$$
I_{\mathrm{dis,raw}}
=
F\int_{A_t}(1+\gamma)\Gamma_B\,dA.
$$

The current input uses:

$$
\gamma=0.1.
$$

The raw target power estimate is:

$$
P_{\mathrm{raw}}
=
|V_{\mathrm{sheath}}|I_{\mathrm{dis,raw}}.
$$

The experimental value `target_discharge_current = 53 A` is used only to
construct a maximum allowed power:

$$
P_{\max}
=
53\ \mathrm{A}\times300\ \mathrm{V}
=
15.9\ \mathrm{kW}.
$$

The power-cap factor is:

$$
C_P
=
\begin{cases}
1,
&P_{\mathrm{raw}}\le P_{\max},\\[4pt]
\dfrac{P_{\max}}{P_{\mathrm{raw}}},
&P_{\mathrm{raw}}>P_{\max}.
\end{cases}
$$

The capped flux is:

$$
\Gamma_{\mathrm{cap}}
=
C_P\overline{\Gamma}_B.
$$

This cap can only reduce an excessive calculated source. It does not increase
a weak calculated discharge toward 53 A. When `see_power_cap_factor = 1`, the
cap is inactive.

### 3. Timestep-Lagged Physical-Response State

The flux supplied to the SEE source is the restartable filtered state
$\Gamma_f$. After an accepted timestep, the exact first-order update is:

$$
\boxed{
\Gamma_f^{n+1}
=
\Gamma_f^n+
\left(1-e^{-\Delta t/\tau}\right)
\left(
\Gamma_{\mathrm{cap}}^n-\Gamma_f^n
\right)
}
$$

where:

- $\Gamma_f$ is the filtered target Cu+ flux.
- $\Gamma_{\mathrm{cap}}$ is the current power-capped target-average Bohm flux.
- $\Delta t$ is the interval between accepted timestep-end updates.
- $\tau=\mathrm{see\_feedback\_response\_time}=10^{-7}\ \mathrm{s}$.

Because `initial_target_ion_flux` is not set in the input, its default is zero:

$$
\Gamma_f(t=0)=0.
$$

The initial execution records the target diagnostics but does not impose a
full-strength SEE source on the seed plasma. The filtered source begins
responding after accepted time advances.

The response fraction is:

$$
\alpha_{\mathrm{response}}
=1-e^{-\Delta t/\tau}.
$$

For example:

$$
\Delta t=\tau
\quad\Longrightarrow\quad
\alpha_{\mathrm{response}}=0.632,
$$

while:

$$
\Delta t=5\tau
\quad\Longrightarrow\quad
\alpha_{\mathrm{response}}=0.993.
$$

Thus, when the numerical timestep is much larger than $10^{-7}$ s, the filter
is effectively a one-accepted-timestep lag. When the timestep is much smaller
than $\tau$, the feedback responds gradually.

The target collector and response object execute at `INITIAL` and
`TIMESTEP_END`. During the next nonlinear solve, $\Gamma_f$ is held fixed. This
avoids putting the complete positive target-ion/SEE loop inside a single
Newton iteration, but it does not remove the physical feedback between
accepted timesteps.

### 4. Spatial Secondary-Electron Carrier Rate

The modeled volumetric secondary-electron carrier rate is:

$$
R_{\mathrm{SEE}}(r,z,t)
=
\gamma\Gamma_f(t)W(r,z).
$$

The spatial weight is:

$$
W(r,z)=W_r(r)W_z(z).
$$

For the top target at $z_t=0.30$ m:

$$
W_z(z)
=
\frac{1}{L_z}
\exp\left[-\frac{z_t-z}{L_z}\right],
$$

with:

$$
L_z=0.012\ \mathrm{m}.
$$

Because the chamber height is $0.30$ m, the omitted finite-domain correction
$1/[1-\exp(-0.30/0.012)]$ differs from one only negligibly.

The active radial profile is an imposed annular Gaussian:

$$
W_r(r)
=
C_r
\exp\left[
-\frac{(r-r_0)^2}{2\sigma_r^2}
\right],
$$

where:

$$
r_0=0.150\ \mathrm{m},
\qquad
\sigma_r=0.035\ \mathrm{m}.
$$

$C_r$ is calculated so that:

$$
\int_0^{R_t}W_r(r)r\,dr
=
\frac{R_t^2}{2},
$$

with:

$$
R_t=0.25\ \mathrm{m}.
$$

Consequently, the integrated source strength is approximately preserved, but
the real calculated radial target-flux distribution is not. The source
location is controlled by the imposed racetrack parameters.

### 5. Represented Cu Ionization Yield

The number of represented Cu ionizations per emitted secondary is:

$$
Y_{\mathrm{iz}}
=
\min\left[
Y_{\max},
\eta_{\mathrm{iz}}
\frac{\max(V_{\mathrm{sheath}}-E_{\mathrm{iz}},0)}
{E_{\mathrm{iz}}}
\right].
$$

The active values are:

$$
\begin{aligned}
V_{\mathrm{sheath}} &= 300\ \mathrm{V},\\
E_{\mathrm{iz}} &= 7.73\ \mathrm{eV},\\
\eta_{\mathrm{iz}} &= 0.25,\\
Y_{\max} &= 12.
\end{aligned}
$$

Therefore:

$$
Y_{\mathrm{iz}}
=
0.25\frac{300-7.73}{7.73}
=
9.4525.
$$

The cap at 12 is not active.

The explicit paired density sources are:

$$
\boxed{
S_{e,\mathrm{SEE}}
=
S_{\mathrm{Cu}^+,\mathrm{SEE}}
=
\gamma\Gamma_fW(r,z)Y_{\mathrm{iz}}
}
$$

in the model's molar source convention. Multiplication by $N_A$ converts them
to particle sources.

The explicit pair-production gain per incident target Cu+ is:

$$
G_{\mathrm{pair}}
=
\gamma Y_{\mathrm{iz}}
=
0.1(9.4525)
=
0.94525.
$$

This value is close to one represented electron-Cu+ pair for every Cu+
reaching the target. The effective closed-loop gain is not exactly 0.94525,
because only a fraction of new ions returns to the target and the plasma also
has bulk ionization and boundary losses. Nevertheless, this near-unity source
factor makes the result sensitive to magnetic confinement, target return
fraction, wall losses, and the fitted value of $\eta_{\mathrm{iz}}$.

### 6. Electron-Energy Source

The energy assigned to each emitted-secondary carrier before the represented
ionization cost is:

$$
E_{\mathrm{abs}}
=
E_{\mathrm{emit}}
+f_{\mathrm{abs}}V_{\mathrm{sheath}}.
$$

The active values are:

$$
E_{\mathrm{emit}}=10\ \mathrm{eV},
\qquad
f_{\mathrm{abs}}=0.7.
$$

Thus:

$$
E_{\mathrm{abs}}
=
10+0.7(300)
=
220\ \mathrm{eV}.
$$

Because:

```text
see_subtract_ionization_energy_from_absorbed_energy = true
```

the thermalized energy deposited per emitted-secondary carrier is:

$$
E_{\mathrm{th}}
=
\max\left(
E_{\mathrm{abs}}-Y_{\mathrm{iz}}E_{\mathrm{iz}},
0
\right).
$$

Numerically:

$$
E_{\mathrm{th}}
=
220-9.4525(7.73)
=
146.93\ \mathrm{eV}.
$$

The explicit bulk electron-energy source is:

$$
\boxed{
Q_{e,\mathrm{SEE}}
=
\gamma\Gamma_fW(r,z)E_{\mathrm{th}}
}
$$

This corresponds to approximately:

$$
\gamma E_{\mathrm{th}}
=
14.69\ \mathrm{eV}
$$

of bulk electron-energy deposition per incident target Cu+.

### 7. Complete Plasma Source Terms

In simplified particle-unit notation:

$$
\frac{\partial n_e}{\partial t}
+\nabla\cdot\boldsymbol{\Gamma}_e
=
S_{\mathrm{bulk}}+S_{e,\mathrm{SEE}},
$$

$$
\frac{\partial n_{\mathrm{Cu}^+}}{\partial t}
+\nabla\cdot\boldsymbol{\Gamma}_{\mathrm{Cu}^+}
=
S_{\mathrm{bulk}}+S_{\mathrm{Cu}^+,\mathrm{SEE}},
$$

where the ordinary local Cu ionization source is:

$$
S_{\mathrm{bulk}}
=
k_{\mathrm{iz}}(\bar{\epsilon}_e)n_en_{\mathrm{Cu}}.
$$

There are therefore two coupled production mechanisms:

1. Local electron-impact Cu ionization, controlled by $n_e$, mean electron
   energy, and the prescribed neutral Cu field.
2. The reduced target-SEE source, controlled by the previous accepted target
   Cu+ Bohm flux.

### 8. What the Target Boundary Actually Does

The target does not use a secondary-electron injection boundary condition.
Instead:

- `SheathEdgeIonBC` removes Cu+ at its natural Bohm/sheath-edge outflow.
- `SheathLimitedElectronBC` removes transmitted bulk electrons.
- `SheathLimitedEnergyBC` removes transmitted bulk electron energy.

The emitted secondary itself is not added to `em` as a boundary electron.
The explicit electron source is:

$$
S_{e,\mathrm{SEE}}
=
R_{\mathrm{SEE}}Y_{\mathrm{iz}},
$$

not:

$$
R_{\mathrm{SEE}}(1+Y_{\mathrm{iz}}).
$$

Thus, the original emitted-secondary electron is represented only through its
modeled ionization products and energy deposition. The discharge-current
diagnostic nevertheless counts the corresponding secondary current through
the factor $(1+\gamma)$. This is a reduced closure, not a fully conservative
kinetic boundary-emission model.

### 9. Neutral Availability in the Active Source

The three active `SEETargetIonFluxSource` kernels do not couple
`neutral_density` and `ion_density`. Therefore:

$$
L_n=1
$$

inside those kernels. There is no smooth neutral-availability limiter reducing
the SEE source.

The input retains a hard `neutral_inventory_guard`. It terminates the run when
the maximum electron-to-neutral or Cu+-to-neutral density ratio exceeds the
configured limit:

$$
\max\left(\frac{n_e}{n_{\mathrm{Cu}}}\right)>1
\quad\text{or}\quad
\max\left(\frac{n_{\mathrm{Cu}^+}}{n_{\mathrm{Cu}}}\right)>1.
$$

`density_ratio_floor = 10^{12} m^{-3}` regularizes these diagnostics. It does
not limit the source.

### 10. Physics Missing from the Reduced SEE Closure

Compared with a real magnetron discharge, the most important missing physics
is:

1. **Ar/Ar+ discharge chemistry and transport.** The feedback driver includes
   only Cu+ impacts, whereas the real target current can contain a large or
   dominant Ar+ contribution.
2. **Species-, energy-, angle-, and surface-dependent emission.** The constant
   $\gamma=0.1$ does not respond to incident ion species, ion energy,
   incidence angle, surface composition, oxidation, coverage, or temperature.
3. **A self-consistent local target sheath.** The source uses a fixed
   $V_{\mathrm{sheath}}=300$ V. It does not use the calculated local,
   time-dependent plasma-to-metal potential difference.
4. **An emitted-electron boundary flux.** The original secondary electron is
   not injected through the target boundary with an energy and angular
   distribution.
5. **Kinetic fast-electron transport.** Acceleration through the sheath,
   gyro-orbits, $E\times B$ motion, magnetic mirroring, nonlocal collisions,
   trapping, and escape are replaced by $W_r(r)W_z(z)$.
6. **Local radial target-flux deposition.** The target-average ion flux is
   redistributed using a fixed annular Gaussian, rather than following the
   solved $\Gamma_B(r)$ profile or secondary trajectories.
7. **A collision-derived ionization cascade.** $\eta_{\mathrm{iz}}=0.25$,
   $Y_{\max}=12$, and the retained-energy fraction are calibration parameters,
   not outputs of an electron energy distribution or Monte Carlo cascade.
8. **A physical multi-timescale feedback response.** One response time
   $\tau$ lumps together ionization, ion return, sheath evolution, and circuit
   response.
9. **External-circuit dynamics.** The model has no power-supply impedance,
   current regulation, target-voltage response, or self-consistent discharge
   power balance. The 53 A value supplies only a downward power ceiling.
10. **Dynamic neutral Cu and sputtering feedback.** Neutral Cu is prescribed
    from a table and is not depleted, transported, or replenished according to
    the evolving target ion flux. Target sputter yield, emitted-neutral energy,
    and angular distributions are not coupled to the plasma.
11. **Additional plasma chemistry.** Excitation, metastables, radiation,
    recombination, charge exchange, Ar-Cu reactions, and multiple Cu charge
    states are absent.
12. **Resolved ion energy and angle at the target.** The Bohm sheath-edge flux
    does not provide the ion energy-angle distribution at the physical target.

Consequently, the present model is appropriate for studying qualitative
transport/feedback trends, but its absolute density, current, and SEE source
strength remain calibration-dependent. With
$\gamma Y_{\mathrm{iz}}\simeq0.945$, the active parameter set is close to a
sensitive feedback regime.

## Historical Current-Normalized Target Flux (Not Active)

> **Historical note:** The remainder of this section through "Historical
> Target SEE Volume Sources" documents the previous 53 A
> current-normalized/conservative-deposition formulation. It is not the model
> selected by either current four-coil or source-only input file.

<details>
<summary>Expand the retired formulation</summary>

`TargetIonFluxSideUserObject` computes the local raw Bohm profile at the target:

$$
\Gamma_B(r,t)=n_{\mathrm{Cu}^+}(r,t)c_s(r,t),
\qquad
c_s=\sqrt{\frac{eT_e+k_BT_i}{m_{\mathrm{Cu}^+}}}.
$$

The raw discharge current includes the ion current and the emitted-secondary
current:

$$
I_{B,\mathrm{raw}}
=q_n\int_{A_t}(1+\gamma)\Gamma_B\,dA,
$$

where `q_n = e*N_A` because the input stores molar densities. The command is

$$
I_{\mathrm{cmd}}(t)=53\tanh\left(\frac{t}{3.0\times10^{-7}}\right)\ \mathrm{A}.
$$

Startup protection applies

$$
C(t)=\min\left[100,
\frac{I_{\mathrm{cmd}}(t)}
{\max(I_{B,\mathrm{raw}},0.1\ \mathrm{A})}\right],
$$

and the target-loss profile used by all consumers is

$$
\Gamma_i^*(r,t)=C(t)\Gamma_B(r,t).
$$

When the current floor or the cap is active, the realized current is
$C I_{B,\mathrm{raw}}$ and can be much smaller than $I_{\mathrm{cmd}}$. The
53 A value is therefore a command, not an automatically guaranteed current
during startup.

The target quadrature points are consolidated into discrete radial annuli. For
annulus `j`, the code retains its physical area and exact emitted rate:

$$
\dot N_{\mathrm{SEC},j}
=C(t)\sum_{q\in j}\gamma_q\Gamma_{B,q}\Delta A_q.
$$

Volume quadrature points are associated with the nearest target annulus. Let
$K_j$ denote the set of volume quadrature points assigned to annulus $j$. The
conservative deposition object first calculates

$$
D_j=\sum_{k\in K_j}W_z(z_k)V_k
$$

and then supplies the local source

$$
S_{\mathrm{SEC},jk}
=\dot N_{\mathrm{SEC},j}\frac{W_z(z_k)}{D_j}.
$$

For one fixed annulus $j$, summing over only its associated volume points gives

$$
\begin{aligned}
\sum_{k\in K_j}S_{\mathrm{SEC},jk}V_k
&=\dot N_{\mathrm{SEC},j}
\frac{\sum_{k\in K_j}W_z(z_k)V_k}{D_j} \\
&=\dot N_{\mathrm{SEC},j}.
\end{aligned}
$$

Now apply $\sum_j$ to both sides of that annulus-level identity:

$$
\sum_j\left(
\sum_{k\in K_j}S_{\mathrm{SEC},jk}V_k
\right)
=\sum_j\dot N_{\mathrm{SEC},j}.
$$

Because every source-region volume point is assigned to exactly one annulus,
the sets $K_j$ form a non-overlapping partition. Therefore the nested sum on
the left is the complete discrete volume integral:

$$
\boxed{
\sum_j\sum_{k\in K_j}S_{\mathrm{SEC},jk}V_k
=\sum_j\dot N_{\mathrm{SEC},j}
}.
$$

The target collector executes in user-object order group `-1`; the volume
normalizer executes in group `0`. Both run on `INITIAL`, `LINEAR`, `NONLINEAR`,
and `TIMESTEP_END`, so source kernels consume a finalized profile from the same
execution stage. SEE rate postprocessors run in group `1`, after the volume
normalizer has finalized, and the relative-difference check runs in group `2`.
This ordering is required: evaluating the volume integral in group `0` would
read a denominator that is still being accumulated.

The radial shape follows the solved target `Cu+` density, bulk electron
temperature, and emission coefficient. Although the input still contains
`see_radial_profile`, `see_target_radial_center`, and
`see_target_radial_width` for the legacy fallback path, they do not control the
active source when `conservative_deposition` is supplied.

## Historical Target SEE Volume Sources (Not Active)

The continuum secondary emission flux at the target is

$$
\Gamma_{\mathrm{SEC}}(r,t)=\gamma\Gamma_i^*(r,t),
\qquad \gamma=0.1.
$$

For the top target, the finite-domain normalized axial deposition profile is

$$
W_z(z)=
\frac{\exp[-(L-z)/\lambda]}
{\lambda[1-\exp(-L/\lambda)]},
\qquad \lambda=0.012\ \mathrm{m}.
$$

The represented ionization yield per emitted secondary electron is:

$$
Y_{\mathrm{iz}}
= \min\!\left[
\eta_{\mathrm{iz}}
\frac{\max\!\left(V_{\mathrm{sheath}}-E_{\mathrm{iz}},\,0\right)}
{E_{\mathrm{iz}}},
Y_{\max}
\right]
$$

where:

- `eta_iz = see_ionization_efficiency = 0.25`
- `E_iz = cu_ionization_energy = 7.73 eV`
- `Y_max = see_max_ionizations_per_secondary = 12`
- `V_sheath = abs(target_plasma_reference - target_metal_voltage) = 300 V`

Their physical meanings are:

- $\gamma=0.1$ is the effective number of emitted secondaries per incident
  target ion.
- $\lambda=0.012\ \mathrm{m}$ is a prescribed deposition-decay length. It is
  not calculated from a local collision cross section in this model.
- $\eta_{\mathrm{iz}}=0.25$ is the assumed fraction of available sheath energy
  represented as Cu ionization events.
- $Y_{\max}=12$ prevents an unbounded number of represented ionizations per
  emitted secondary.
- $E_{\mathrm{emit}}=10\ \mathrm{eV}$ is the initial kinetic-energy allowance
  assigned to each emitted secondary.
- $\alpha_{\mathrm{abs}}=0.7$ is the assumed fraction of the 300 V target drop
  retained in the modeled bulk electron energy.
- $n_f=10^{12}\ \mathrm{m^{-3}}$ regularizes the neutral limiter near zero
  density.

With the neutral-availability limiter `L_n`, the paired fast-beam sources are:

$$
\begin{aligned}
n_{\mathrm{Cu}}^* &=
\max\!\left(n_{\mathrm{Cu}}-n_f,0\right)+n_f, \\
L_n &=
\frac{n_{\mathrm{Cu}}^*}
{n_{\mathrm{Cu}}^*+n_{\mathrm{Cu}^+}+n_f},
\qquad n_f=10^{12}\ \mathrm{m^{-3}}.
\end{aligned}
$$

$$
\begin{aligned}
S_{e,\mathrm{SEE}}
&= \Gamma_{\mathrm{SEC}}Y_{\mathrm{iz}}L_nW_z, \\
S_{\mathrm{Cu}^+,\mathrm{SEE}}
&= S_{e,\mathrm{SEE}} .
\end{aligned}
$$

In the implementation, $\Gamma_{\mathrm{SEC}}W_z$ in these equations is evaluated by the
conservative annular source above. The `see_rate_relative_difference`
postprocessor compares its volume integral with `target_see_rate`; it should be
near roundoff for the active `position_units = 1` geometry. This check applies
to the emitted-secondary carrier rate before
$Y_{\mathrm{iz}}$ and $L_n$ are applied. The electron and Cu+ kernels then use
the same pair source, so the prescribed ionization is charge-pair balanced.

For the active constants,

$$
Y_{\mathrm{iz}}
=0.25\frac{300-7.73}{7.73}
=9.4525,
$$

so the cap at 12 is not active. The emitted secondary itself is not added as a
separate electron to `em`; `em` receives only these represented ionization-pair
electrons after the unresolved beam has acted.

The boundary emission-energy source has been removed, so the volume source
includes both the initial emission energy and retained sheath energy:

$$
S_{\epsilon,\mathrm{SEE}}
= \Gamma_{\mathrm{SEC}}L_nW_z
\max\!\left(
E_{\mathrm{emit}}+\alpha_{\mathrm{abs}}V_{\mathrm{sheath}}
-Y_{\mathrm{iz}}E_{\mathrm{iz}},
0
\right)
$$

with `E_emit = 10 eV` and `alpha_abs = 0.7`.

The subtraction occurs because:

$$
\mathrm{see\_subtract\_ionization\_energy\_from\_absorbed\_energy}
= \mathrm{true}
$$

So the model prevents the same secondary-electron energy from being counted
both as explicit ionization energy and as bulk thermal energy.

At the active constants, the retained bulk energy per emitted secondary is

$$
10+0.7(300)-9.4525(7.73)=146.93\ \mathrm{eV}.
$$

</details>

## Boundary Conditions

### Potential

The target plasma boundary is fixed at:

$$
\phi_{\mathrm{target}}
= \phi_{\mathrm{target,ref}}
= 0\ \mathrm{V}
$$

The target metal voltage is not part of Poisson's equation. It only sets the
effective SEE sheath energy:

$$
\begin{aligned}
V_{\mathrm{sheath}}
&= \max\!\left(
\phi_{\mathrm{target,ref}} - V_{\mathrm{target,metal}},
0
\right) \\
&= \max\!\left(0 - (-300),\,0\right) \\
&= 300\ \mathrm{V}.
\end{aligned}
$$

The wafer and bottom shield are prescribed directly as plasma-potential
Dirichlet boundaries:

$$
\phi_{\mathrm{wafer}}(t)
= V_{\mathrm{dc}}
+ \tanh\!\left(\frac{t}{t_{\mathrm{ramp}}}\right)
V_{\mathrm{rf}}
\sin\!\left(2\pi f_{\mathrm{rf}}t\right)
$$

With the input values:

$$
\phi_{\mathrm{wafer}}(t)
= -75\ \mathrm{V},
$$

because `wafer_rf_voltage = 0` in the active input. The 13.56 MHz waveform
machinery remains configured but contributes no RF voltage unless that
amplitude is changed.

This is a reduced bulk-field drive. It is not a resolved wafer sheath model.

### Electron And Electron-Energy Losses

`SheathLimitedElectronBC` removes only bulk electrons according to

$$
\Gamma_e=
f_{\mathrm{loss}}(t)\frac{1-r}{1+r}
\frac14n_ev_{\mathrm{th},e}
\exp\!\left[-\frac{\max(\phi_p-V_m,0)}{T_e}\right],
$$

where

$$
v_{\mathrm{th},e}=\sqrt{\frac{8eT_e}{\pi m_e}},
\qquad
f_{\mathrm{loss}}(t)=\tanh\!\left(\frac{t}{t_{\mathrm{loss,ramp}}}\right).
$$

At the target and right wall, $r=0$ and the loss scale is one.

`SheathLimitedEnergyBC` applies the matching transmission factor to bulk
electron-energy loss. The target metal voltage is used only in this unresolved
sheath barrier and is not imposed in Poisson.

The wafer and bottom shield retain Hagelaar-type kinetic boundary conditions.
Define the magnetized electron drift velocity and its outward normal component
as

$$
u_{d,e,n}
=\mathcal M_B(-\mu_e\mathbf E)\cdot\mathbf n,
\qquad
a_e=\begin{cases}1,&u_{d,e,n}>0,\\0,&u_{d,e,n}\le0.\end{cases}
$$

The implemented particle loss is

$$
\Gamma_{e,H}
=f_{\mathrm{loss}}(t)\frac{1-r}{1+r}
\left[(2a_e-1)u_{d,e,n}+\frac12v_{\mathrm{th},e}\right]n_e.
$$

For electron energy, with
$u_{d,\epsilon,n}=\mathcal M_B(-\mu_\epsilon\mathbf E)\cdot\mathbf n$ and the
corresponding direction switch $a_\epsilon$, the implemented loss is

$$
\Gamma_{\epsilon,H}
=f_{\mathrm{loss}}(t)\frac{1-r}{1+r}
\left[(2a_\epsilon-1)u_{d,\epsilon,n}
+\frac56v_{\mathrm{th},e}\right]n_\epsilon.
$$

Their reflection coefficient `r` therefore appears as:

$$
\mathrm{loss\ factor} \propto \frac{1-r}{1+r}
$$

For those boundaries:

- `r = 0` means fully absorbing.
- `r` close to `1` means nearly reflecting.

The input uses:

$$
\begin{aligned}
r_{\mathrm{wafer}} &= 0.5, \\
r_{\mathrm{bottom\ shield}} &= 0.5 .
\end{aligned}
$$

At the right computational boundary, Poisson retains its natural condition:

$$
\mathbf n\cdot\epsilon\nabla\phi=0.
$$

The solved value $\phi_s=\phi(r=R,z)$ is the plasma-side sheath-edge
potential, not the metal voltage. The external wall metal is fixed at
$V_{\mathrm{wall}}=0\ \mathrm{V}$ in the same voltage reference used by the
target plasma and metal functions. `SheathLimitedElectronBC` applies

$$
\Gamma_{e,w}=\frac14n_ev_{\mathrm{th},e}
\exp\left[-\frac{\max(\phi_s-V_{\mathrm{wall}},0)}{T_e}\right],
$$

and `SheathLimitedEnergyBC` applies the matching retarded energy flux:

$$
\Gamma_{\epsilon,w}=\frac53\bar\epsilon_e\Gamma_{e,w}.
$$

Boundary losses are ramped by:

$$
\tanh\!\left(\frac{t}{t_{\mathrm{loss,ramp}}}\right)
$$

with:

$$
t_{\mathrm{loss,ramp}} = 5.0\times10^{-8}\ \mathrm{s}
$$

### Cu+ Boundary Losses

The target uses an unnormalized `SheathEdgeIonBC`:

$$
\Gamma_{\mathrm{Cu}^+,t}
=n_{\mathrm{Cu}^+}c_s.
$$

It is the natural absorbing Bohm/sheath-edge loss that is independently
measured by `TargetIonFluxSideUserObject`. The SEE feedback source does not
multiply or replace this boundary loss.

The right wall also uses unnormalized `SheathEdgeIonBC`, including the loss
ramp:

$$
\Gamma_{\mathrm{Cu}^+,w}
=\tanh\!\left(\frac{t}{t_{\mathrm{loss,ramp}}}\right)
n_{\mathrm{Cu}^+}c_s.
$$

The wafer and bottom shield retain `LymberopoulosIonBC` with their configured
loss scales. Its implemented normal ion flux is

$$
\Gamma_{i,L}
=f_{\mathrm{ion}}f_{\mathrm{loss}}(t)
\mu_i(\mathbf E\cdot\mathbf n)n_i.
$$

Unlike the Bohm sheath-edge BC, this expression follows the local drift field
and does not clamp a field pointing away from the wall. The configured scales
are:

$$
\begin{aligned}
f_{\mathrm{ion,wafer}} &= 0.5, \\
f_{\mathrm{ion,bottom\ shield}} &= 0.5 .
\end{aligned}
$$

The right-wall diagnostics report the minimum, area average, and maximum of

$$
\Delta\phi_w=\phi_s-V_{\mathrm{wall}}.
$$

An ion-sheath interpretation requires this signed drop to remain generally
positive; the electron BC itself uses $\max(\Delta\phi_w,0)$.

## Transport Coefficients

### Electron Transport

`ElectronTransportCoefficients` reads:

```text
rate_coefficients_cu/electron_moments.txt
```

The file columns are:

```text
mean electron energy [eV],
mobility times neutral density,
diffusion times neutral density
```

The input has:

$$
\begin{aligned}
\mathrm{interp\_trans\_coeffs} &= \mathrm{true}, \\
\mathrm{pressure\_dependent\_electron\_coeff} &= \mathrm{true}, \\
p_{\mathrm{gas}} &= p_{\mathrm{Cu,local}} .
\end{aligned}
$$

Therefore Zapdos samples electron mobility and diffusion versus:

$$
\epsilon = \exp(\mathrm{mean\_en}-\mathrm{em})
$$

and scales them with:

$$
N^{-1} = \frac{k_B T_{\mathrm{gas}}}{p_{\mathrm{Cu,local}}}
$$

Equivalently, if the table stores the reduced moments $(\mu_eN)_{\mathrm{tab}}$
and $(D_eN)_{\mathrm{tab}}$, the material evaluates

$$
\begin{aligned}
\mu_e(\epsilon,p)
&=(\mu_eN)_{\mathrm{tab}}(\epsilon)
\frac{k_BT_{\mathrm{gas}}}{p_{\mathrm{Cu,local}}}, \\
D_e(\epsilon,p)
&=(D_eN)_{\mathrm{tab}}(\epsilon)
\frac{k_BT_{\mathrm{gas}}}{p_{\mathrm{Cu,local}}}.
\end{aligned}
$$

The underlying BOLSIG+ calculation is performed over a range of reduced fields
$E/N$. Every BOLSIG+ solution supplies both its mean energy and its transport
and reaction moments. `scripts/convert_bolsig_cu.py` re-parameterizes those
results as functions of mean energy:

$$
\frac{E}{N}
\longrightarrow
\left\{
\epsilon,\ \mu_eN,\ D_eN,\ k_{\mathrm{iz}}
\right\}
\longrightarrow
\left\{
\mu_eN(\epsilon),\ D_eN(\epsilon),\ k_{\mathrm{iz}}(\epsilon)
\right\}.
$$

During the Zapdos solve, the instantaneous local $E/N$ is therefore not used
to index these tables directly. The mean-energy equation evolves $\epsilon$,
and the local-mean-energy approximation uses that $\epsilon$ to select the
coefficients. The electric field influences them indirectly through Joule
heating and the evolved mean energy.

The mean energy is clamped between `0.01 eV` and `100 eV` for transport-table
interpolation and in the Hagelaar wafer/bottom-shield boundary models. The
target/right-wall sheath-limited BCs and the Bohm target collector use the
solved mean energy without this clamp.

### Cu+ Transport

`ADHeavySpecies` sets Cu+ mass and mobility:

$$
\begin{aligned}
m_{\mathrm{Cu}^+} &= 1.0552069\times10^{-25}\ \mathrm{kg}, \\
z_{\mathrm{Cu}^+} &= +1, \\
T_{\mathrm{Cu}^+} &= 300\ \mathrm{K}.
\end{aligned}
$$

The reduced mobility is:

$$
K_0 = 2.2\times10^{-4}\ \mathrm{m^2\,V^{-1}\,s^{-1}}
$$

with reference conditions:

$$
p_{\mathrm{ref}} = 101325\ \mathrm{Pa},
\qquad
T_{\mathrm{ref}} = 273.15\ \mathrm{K}
$$

Zapdos computes:

$$
\mu_{\mathrm{Cu}^+}
= K_0
\left(\frac{p_{\mathrm{ref}}}{p_{\mathrm{Cu,local}}}\right)
\left(\frac{T_{\mathrm{Cu}^+}}{T_{\mathrm{ref}}}\right)
$$

and then computes diffusion from the Einstein relation:

$$
D_{\mathrm{Cu}^+}
= \frac{\mu_{\mathrm{Cu}^+}k_B T_{\mathrm{Cu}^+}}{e}
$$

## Prescribed Neutral Cu And Pressure

The table-loaded neutral density is:

$$
n_{\mathrm{Cu,table}}(r,z)
$$

from:

```text
runs/zapdos_hpem_rz_30cm/moose_tables/n_Cu_m3.tbl
```

The effective neutral density used in the model is:

$$
\begin{aligned}
n_{\mathrm{Cu,eff}}(r,z)
&= M_{\mathrm{Cu}}
\max\!\left(n_{\mathrm{Cu,table}}(r,z),n_{\mathrm{floor}}\right)
+ n_{\mathrm{bg}} \\
&\quad
+ n_{\mathrm{peak}}
\exp\!\left[-\left(\frac{r-r_0}{w_r}\right)^2\right]
\exp\!\left[-\left(\frac{z-z_0}{w_z}\right)^2\right].
\end{aligned}
$$

with:

$$
\begin{aligned}
M_{\mathrm{Cu}} &= 0.1, \\
n_{\mathrm{floor}} &= 1.0\times10^{16}\ \mathrm{m^{-3}}, \\
n_{\mathrm{bg}} &= 1.0\times10^{17}\ \mathrm{m^{-3}}, \\
n_{\mathrm{peak}} &= 3.0\times10^{18}\ \mathrm{m^{-3}}, \\
r_0 &= 0.235\ \mathrm{m}, \\
z_0 &= 0.245\ \mathrm{m}, \\
w_r &= 0.080\ \mathrm{m}, \\
w_z &= 0.090\ \mathrm{m}.
\end{aligned}
$$

The pressure used for electron and Cu+ transport is:

$$
p_{\mathrm{Cu,local}}
= p_{\mathrm{Ar,bg}}
+ k_B T_{\mathrm{gas}} n_{\mathrm{Cu,eff}}
$$

where:

$$
p_{\mathrm{Ar,bg}} = 10\ \mathrm{Pa},
\qquad
T_{\mathrm{gas}} = 300\ \mathrm{K}
$$

The name `argon_background_pressure` is used as a residual pressure floor in
the transport coefficient calculation. Ar+ chemistry itself is not active.

## Initial Conditions

The initial plasma density is a weak seed plus a near-target magnetron seed:

$$
\begin{aligned}
n_{\mathrm{seed}}(r,z)
&= n_{\mathrm{floor,plasma}}
+ n_{\mathrm{bulk,peak}}
\left(1-\frac{r^2}{R^2}\right)^2
\left(\frac{z}{L}\right)^2
\left(1-\frac{z}{L}\right)^2 \\
&\quad
+ n_{\mathrm{mag,peak}}
\exp\!\left[-\left(\frac{r-r_m}{w_r}\right)^2\right]
\exp\!\left[-\left(\frac{z-z_m}{w_z}\right)^2\right].
\end{aligned}
$$

with:

$$
\begin{aligned}
n_{\mathrm{floor,plasma}} &= 1.0\times10^{14}\ \mathrm{m^{-3}}, \\
n_{\mathrm{bulk,peak}} &= 3.0\times10^{14}\ \mathrm{m^{-3}}, \\
n_{\mathrm{mag,peak}} &= 2.0\times10^{15}\ \mathrm{m^{-3}}, \\
r_m &= 0.235\ \mathrm{m}, \\
z_m &= 0.275\ \mathrm{m}, \\
w_r &= 0.035\ \mathrm{m}, \\
w_z &= 0.025\ \mathrm{m}.
\end{aligned}
$$

The initial mean energy density uses:

$$
\epsilon_{\mathrm{bulk,initial}} = 6\ \mathrm{eV},
\qquad
\epsilon_{\mathrm{magnetron,initial}} = 8\ \mathrm{eV}
$$

The initial potential is a linear interpolation from wafer DC bias to target
plasma reference:

$$
\phi_{\mathrm{initial}}(z)
= V_{\mathrm{wafer,dc}}
+ \left(\phi_{\mathrm{target,ref}}-V_{\mathrm{wafer,dc}}\right)
\frac{z}{L}
$$

## Prescribed Tables And How They Are Generated

The input comments say to generate analytic HPEM-like R-Z tables with:

```bash
python scripts/generate_hpem_rz_30cm_tables.py
```

The generator writes:

```text
runs/zapdos_hpem_rz_30cm/moose_tables/n_Cu_m3.tbl
runs/zapdos_hpem_rz_30cm/moose_tables/S_iz_Cu_m3_s.tbl
runs/zapdos_hpem_rz_30cm/moose_tables/S_Cu_eff_m3_s.tbl
runs/zapdos_hpem_rz_30cm/moose_tables/Qe_eff_eV_m3_s.tbl
runs/zapdos_hpem_rz_30cm/moose_tables/Bx_T.tbl
runs/zapdos_hpem_rz_30cm/moose_tables/By_T.tbl
```

The generator builds:

- an analytic neutral Cu plume near the top racetrack,
- a synthetic magnetron field near the target,
- an external four-coil guide field,
- optional analytic HPEM-like fast-electron/ionization source maps.

With `--magnetron-model two-loop`, the source field is an image-calibrated
surrogate constructed from three finite-width annular magnet columns behind
the target. Each column contains opposite front and back virtual magnetic
poles, so it has no magnetic-monopole far field. The three column amplitudes
are solved so the field has a null near

$$
(r_{\mathrm{null}},z_{\mathrm{null}})=(0.232,0.190)\ \mathrm{m},
$$

then the complete field is normalized to a peak magnitude of $0.10\ \mathrm{T}$.
This construction produces two target-side arches and a predominantly axial
fan below the null. It is calibrated to the supplied field image; it is not a
replacement for measured or magnetostatic-solver $B_r(r,z)$ and $B_z(r,z)$
data.

The BOLSIG-derived transport and Cu ionization files are generated separately
with:

```bash
python scripts/convert_bolsig_cu.py
```

from `rate_coefficients_cu/cu_bolsig_energy.dat` and the cleaned Cu
cross-section input referenced by `rate_coefficients_cu/README.md`.

However, this specific input actively loads only:

```text
n_Cu_m3.tbl
Bx_T.tbl
By_T.tbl
```

It does not load `S_iz_Cu_m3_s.tbl`, `S_Cu_eff_m3_s.tbl`, or
`Qe_eff_eV_m3_s.tbl` into source kernels. Those source tables are generated on
disk, but the active volumetric plasma source in this input is the live
target-ion-flux-driven SEE closure plus Cu electron-impact ionization.

## Main Assumptions

1. The chamber is axisymmetric and modeled in R-Z coordinates.
2. The chemistry is Cu-only: electrons, Cu+, and prescribed neutral Cu.
3. Neutral Cu is not solved as a continuity equation in this input.
4. Electron transport is magnetized; Cu+ transport is unmagnetized.
5. The target sheath is not resolved.
6. The target metal voltage is outside Poisson and only sets the SEE energy
   scale.
7. The wafer waveform machinery is imposed directly as a plasma-potential
   Dirichlet boundary, so the model does not predict the wafer sheath. Its
   active RF amplitude is zero, leaving a constant $-75\ \mathrm{V}$ boundary.
8. Target SEE is represented by paired bulk volume sources driven by a
   timestep-lagged, power-capped target-average Cu+ Bohm flux. The fast beam
   itself is not a solved species.
9. The right metal is grounded at 0 V outside Poisson, while its plasma-side
   sheath-edge potential remains solved with a natural Poisson boundary.
10. Wafer and bottom-boundary reflection/loss coefficients are
    stabilization/reduced-model controls, not sheath diagnostics.
11. The analytic HPEM-like source tables exist but are not active source terms
    in this input file.
12. SEE deposition uses a prescribed normalized annular Gaussian in radius and
    an exponential decay below the target. It does not follow the calculated
    local target-flux profile or fast-electron trajectories along curved
    magnetic field lines.
13. The target Bohm flux and filtered response are stored as `Real` user-object
    data. They update at accepted timestep end and are held fixed during the
    next nonlinear solve, so the feedback is timestep-lagged and its derivatives
    are not included in the automatic-differentiation Jacobian.
14. The experimental 53 A value does not prescribe or boost the target current.
    It defines a 15.9 kW downward-only cap. The model does not predict the
    external circuit or guarantee 53 A.
15. The active SEE kernels have no smooth neutral limiter. A hard inventory
    guard only terminates the calculation when charged density becomes
    comparable to or exceeds the fixed effective neutral Cu density.
