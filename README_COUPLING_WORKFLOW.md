# Cu PVD Zapdos-MC Hybrid Coupling Workflow

This file explains the coupling process between Zapdos and Monte Carlo (MC)
for the Cu PVD model. The original `README.md` is kept unchanged.

The most important point is that the **first pass is not fully
self-consistent yet**. At the beginning, the model does not know the true
Cu ionization source, because that depends on the neutral Cu density and the
Zapdos electron fields. Therefore the first pass uses:

- an initial prescribed target Cu source
- an initial prescribed Cu ionization source

After Zapdos and Cu+ MC have run once, both sources can be updated and the
coupling loop becomes self-consistent.

## 1. Short Answer: What Runs First?

Run the model in this order:

```text
1. Build initial target Cu source.
2. Run neutral Cu MC.
3. Build initial prescribed Cu ionization source.
4. Send n_Cu, S_iz, B_total, and operating conditions to Zapdos.
5. Run Zapdos to get plasma fields.
6. Run Cu+ MC using Zapdos fields.
7. Update target Cu source from Cu+ return to target.
8. Update Cu ionization source from n_Cu, ne, and Te.
9. Relax/smooth the updated fields.
10. Repeat from the next Zapdos solve until converged.
```

In compact form:

```text
initial target source
  -> neutral Cu MC
  -> n_Cu(r,z)
  -> initial S_iz(r,z)
  -> Zapdos
  -> phi, E, ne, Te
  -> Cu+ MC
  -> Cu+ target return
  -> updated target source and updated S_iz
  -> repeat
```

Your understanding is correct:

```text
Use the initial prescribed target source and ionization source first.
Run MC to get Cu density and fluxes.
Input those fields into Zapdos to calculate plasma fields.
Use the Zapdos fields to run Cu+ MC.
Use Cu+ MC results to update the sources.
```

The detail to keep in mind is that there are **two MC solves**:

- Neutral Cu MC runs first to get `n_Cu(r,z)`.
- Cu+ MC runs after Zapdos, because Cu+ trajectories need `E(r,z)`.

## 2. What Each Solver Does

### Neutral Cu MC

Neutral Cu MC follows sputtered neutral copper atoms from the target.

It needs:

```text
Gamma_Cu_target(r)     target neutral Cu source
B_total(r,z)           optional field information for source shaping
geometry               target, wafer, wall locations
```

It produces:

```text
n_Cu(r,z)              neutral copper density
Gamma_Cu0_wafer(r)     neutral Cu flux to wafer
Gamma_Cu0_wall         neutral Cu loss to wall
```

The most important output for coupling is:

```text
n_Cu(r,z)
```

Zapdos needs this field because Cu ionization depends on neutral Cu density.

### Zapdos

Zapdos solves the plasma/electron-fluid part.

It needs:

```text
n_Cu(r,z)              neutral copper density from neutral MC
S_iz_Cu(r,z)           Cu ionization source estimate
B_total(r,z)           total magnetic field
target bias            target boundary condition
mesh and geometry      same r-z coordinate convention as MC
boundary fluxes        target, wafer, and wall flux estimates
```

It produces:

```text
phi(r,z)               electric potential
E(r,z)                 electric field
ne(r,z)                electron density
Te(r,z)                electron temperature or mean electron energy
```

The most important outputs for coupling are:

```text
E(r,z), ne(r,z), Te(r,z)
```

Cu+ MC needs `E(r,z)` to push ions, and the coupler needs `ne` and `Te` to
update the Cu ionization source.

### Cu+ MC

Cu+ MC follows copper ions born from the ionization source.

It needs:

```text
S_iz_Cu(r,z)           Cu+ birth source
E(r,z)                 electric field from Zapdos
B_total(r,z)           total magnetic field
geometry               target, wafer, wall locations
```

It produces:

```text
Gamma_Cu+_target(r)    Cu+ return flux to target
Gamma_Cu+_wafer(r)     Cu+ flux to wafer
Gamma_Cu+_wall         Cu+ loss to wall
IEDF_wafer             wafer ion energy distribution
angle_wafer            wafer ion angular distribution
```

The most important output for coupling is:

```text
Gamma_Cu+_target(r)
```

Returning Cu+ ions sputter more Cu from the target, so this flux updates the
next target Cu source.

## 3. The First Pass

The first pass creates the first meaningful Zapdos input package.

### Step 1: Build Initial Target Cu Source

The model starts from the measured target current and target voltage.

The target ion flux estimate is:

```text
Gamma_i,target = I_target / [e A_erosion (1 + gamma_SE)]
```

The approximate ion impact energy is:

```text
E_i ~= e(V_plasma - V_target)
```

The initial sputtered Cu source is:

```text
Gamma_Cu_target_initial(r)
  = Y_Cu(E_i) Gamma_i,target racetrack_shape(r)
```

This gives the first neutral Cu source at the target.

Code:

```text
coupling/initialize.py
```

### Step 2: Run Neutral Cu MC

Use `Gamma_Cu_target_initial(r)` to launch neutral Cu atoms.

Neutral Cu MC gives:

```text
n_Cu_initial(r,z)
Gamma_Cu0_wafer(r)
Gamma_Cu0_wall
```

At this point the model knows where neutral Cu atoms are likely to be in the
chamber, but it still does not know the self-consistent ionization source.

Code:

```text
mc/neutral_cu_mc.py
```

### Step 3: Build Initial Prescribed Ionization Source

Before Zapdos has solved the plasma fields, the code uses a prescribed
ionization source:

```text
S_iz_initial(r,z) = S0 F_B(r,z) F_r(r) exp(-z / L_ion)
```

It is normalized using the total Cu source:

```text
integral S_iz_initial dV = f_ion Q_Cu
```

This field is only the starting guess. It lets Zapdos and Cu+ MC complete the
first pass.

Code:

```text
coupling/source_update.py
```

### Step 4: Write the First Zapdos Input Package

The first Zapdos input package contains:

```text
n_Cu_initial(r,z)          from neutral Cu MC
S_iz_initial(r,z)          prescribed starting ionization source
B_total(r,z)               magnetic field
Gamma_Cu_target_initial    target source profile
boundary flux estimates    neutral fluxes; Cu+ fluxes may be zero initially
operation settings         target bias, current, power, pressure, RF settings
mesh                       shared r-z mesh
```

These are written as files such as:

```text
mc_fields.csv
target_source.csv
boundary_fluxes.csv
wall_losses.csv
operation.csv
mesh.csv
zapdos_input_manifest.yaml
```

Code:

```text
coupling/zapdos_io.py
README_ZAPDOS_INPUTS.md
```

### Step 5: Run Zapdos

Zapdos reads the MC/coupler fields and solves for plasma fields:

```text
phi(r,z)
E(r,z)
ne(r,z)
Te(r,z)
```

These fields are sent back to the coupler.

Zapdos template:

```text
zapdos_templates/cu_pvd_hybrid_template.i
```

### Step 6: Run Cu+ MC

Now Cu+ MC can run, because the electric field is available.

Cu+ MC uses:

```text
S_iz_initial(r,z)          Cu+ birth locations for the first pass
E(r,z)                     electric field from Zapdos
B_total(r,z)               total magnetic field
```

Cu+ ions are pushed with:

```text
m dv/dt = q(E + v x B_total)
```

Cu+ MC returns:

```text
Gamma_Cu+_target(r)
Gamma_Cu+_wafer(r)
Gamma_Cu+_wall
IEDF_wafer
angle_wafer
```

Code:

```text
mc/ion_cu_mc.py
mc/boris.py
```

### Step 7: Update the Sources

After the first Cu+ MC pass, the model can update both main sources.

The target source is updated from Cu+ target return:

```text
Gamma_Cu_target_new(r)
  = Y_Cu(E_i,target) Gamma_Cu+_target(r)
  + optional Ar+ sputter contribution
```

The ionization source is updated from the neutral density and Zapdos electron
fields:

```text
S_iz_new(r,z) = n_Cu(r,z) ne(r,z) k_iz(Te)
```

This is the first point where the model has all pieces needed for a coupled
update.

Code:

```text
coupling/source_update.py
```

## 4. Later Coupled Iterations

After the first pass, the model no longer relies only on the prescribed
initial sources. Each later iteration uses relaxed fields from the previous
iteration.

The repeated loop is:

```text
1. Start with relaxed Gamma_Cu_target(r) and S_iz(r,z).
2. Run neutral Cu MC using Gamma_Cu_target(r).
3. Use the new neutral density n_Cu(r,z).
4. Write n_Cu, S_iz, B_total, and boundary data to Zapdos.
5. Run Zapdos to get phi, E, ne, and Te.
6. Run Cu+ MC using S_iz, E, and B_total.
7. Update Gamma_Cu_target(r) from Cu+ target return.
8. Update S_iz(r,z) from n_Cu, ne, and k_iz(Te).
9. Smooth noisy MC tallies.
10. Under-relax n_Cu, S_iz, and Gamma_Cu_target.
11. Check convergence.
```

The feedback loop is:

```text
more Cu+ returns to target
  -> more Cu sputtering
  -> more neutral Cu
  -> more Cu ionization
  -> more Cu+
  -> possibly more Cu+ target return
```

The magnetic field affects this loop by changing:

```text
Cu+ confinement
Cu+ target return probability
Cu+ wall loss
Cu+ wafer arrival energy
Cu+ wafer arrival angle
electron transport in Zapdos
```

## 5. Relaxation and Convergence

The raw MC tallies are noisy, and the target-source feedback can be strong.
The code therefore smooths and under-relaxes updates.

For neutral Cu density:

```text
n_Cu_next = (1 - alpha) n_Cu_old + alpha n_Cu_MC
```

For ionization source:

```text
S_iz_next = (1 - alpha) S_iz_old + alpha S_iz_new
```

For target source:

```text
Gamma_target_next =
    (1 - alpha) Gamma_target_old
  + alpha Gamma_target_new
```

Typical initial values are:

```text
alpha = 0.05 to 0.2
```

The coupled solution is considered converged when these stop changing within
tolerance:

```text
target current error
wafer Cu flux change
Cu+ target flux change
total ionization change
source-loss balance error
```

Code:

```text
coupling/relaxation.py
coupling/convergence.py
```

## 6. Data Exchange Between MC and Zapdos

The MC-to-Zapdos package contains these main files.

### `mc_fields.csv`

Cell-centered fields on the shared `r-z` mesh:

```text
r_m
z_m
n_Cu_m3
S_iz_Cu_m3_s
Br_T
Btheta_T
Bz_T
Bmag_T
```

The most important columns for Zapdos are:

```text
n_Cu_m3
S_iz_Cu_m3_s
Br_T, Btheta_T, Bz_T
```

### `target_source.csv`

Radial target sputtering source:

```text
r_m
Gamma_Cu_target_m2_s
```

### `boundary_fluxes.csv`

Radial target and wafer flux estimates:

```text
r_m
Gamma_Cu0_target_return_m2_s
Gamma_Cu0_wafer_m2_s
Gamma_Cuplus_target_m2_s
Gamma_Cuplus_wafer_m2_s
```

In the first Zapdos package, Cu+ fluxes may be zero because Cu+ MC has not
run yet.

### `operation.csv`

Operating conditions:

```text
target_bias_V
plasma_potential_guess_V
target_current_A
target_power_W
wafer_rf_amplitude_V
wafer_rf_frequency_Hz
wafer_rf_ramp_time_s
pressure_Ar_Torr
```

### `mesh.csv`

Shared mesh information:

```text
index_r
index_z
r_center_m
z_center_m
cell_volume_m3
```

## 7. Coordinate Convention

The geometry is axisymmetric:

```text
r = radial coordinate
z = axial coordinate
```

The chamber convention is:

```text
z = 0                  target
z = target_to_wafer    wafer
r = chamber_radius     side wall
```

The mesh settings are in:

```text
config/case.yaml
```

Relevant mesh fields:

```text
mesh.nr
mesh.nz
mesh.r_max
mesh.z_max
```

## 8. Magnetic Field Handling

The model keeps three B-field maps:

```text
B_magnetron   field from the magnetron
B_external    externally applied field
B_total       B_magnetron + B_external
```

Both Zapdos and MC use the same `B_total(r,z)` after interpolation to the
shared mesh.

Code:

```text
fields/bmap_reader.py
fields/interpolation.py
```

## 9. Practical Manual Order

If running the pieces manually, the conceptual order is:

```text
1. Generate or choose Gamma_Cu_target_initial(r).
2. Run neutral Cu MC to get n_Cu_initial(r,z).
3. Generate S_iz_initial(r,z).
4. Write the Zapdos input package.
5. Convert MC CSV fields to MOOSE table format if the Zapdos input requires it.
6. Run Zapdos with the generated tables and template input file.
7. Read Zapdos fields back into the coupler.
8. Run Cu+ MC.
9. Update target source and ionization source.
10. Repeat the coupled iteration.
```

The helper for creating a standalone initial Zapdos input package is:

```bash
python scripts/prepare_zapdos_inputs.py \
  --config config/case.yaml \
  --out runs/zapdos_initial_input
```

The helper for converting `mc_fields.csv` to MOOSE tables is:

```bash
python scripts/convert_mc_fields_to_moose_tables.py \
  --mc-fields runs/zapdos_initial_input/mc_fields.csv \
  --out-dir runs/zapdos_initial_input/moose_tables
```

Then use:

```text
zapdos_templates/cu_pvd_hybrid_template.i
```

## 10. Main Code Map

```text
config/case.yaml                 main case settings
fields/bmap_reader.py            B_magnetron, B_external, B_total handling
fields/interpolation.py          mesh, interpolation, conservative smoothing
fields/zapdos_field_reader.py    Zapdos output field reader helpers
coupling/initialize.py           initial target source and mesh setup
coupling/source_update.py        S_iz and target source updates
coupling/zapdos_io.py            Zapdos file exchange
coupling/relaxation.py           smoothing and under-relaxation
coupling/convergence.py          convergence checks
coupling/driver.py               full coupling loop
mc/neutral_cu_mc.py              neutral Cu MC
mc/ion_cu_mc.py                  Cu+ MC
mc/boris.py                      Boris charged-particle pusher
post/diagnostics.py              iteration diagnostics
post/plots.py                    plotting utilities
scripts/prepare_zapdos_inputs.py standalone Zapdos input package generator
scripts/convert_mc_fields_to_moose_tables.py MOOSE table converter
```

## 11. Key Outputs

During coupled iterations, the main quantities to inspect are:

```text
total Cu source
total Cu ionization
Cu+ target flux
Cu+ wafer flux
neutral wafer flux
wall loss
I_target_sim
deposition profile
Cu+ ion fraction at wafer
wafer ion energy distribution
wafer angular distribution
```

Useful field plots are:

```text
n_Cu(r,z)
S_iz(r,z)
ne(r,z)
phi(r,z)
radial target and wafer flux profiles
```

## 12. Current Simplifying Assumptions

The present model is still a prototype. Important assumptions are:

- Axisymmetric `r-z` geometry.
- Prescribed magnetic field; the plasma does not modify `B`.
- Neutral Cu transport is initially ballistic.
- Cu-Cu, Cu-Ar, and charge-exchange collisions are not fully included.
- Surface interactions are simple sticking, collection, or neutralization.
- Cu+ birth velocity is thermal or parent-neutral approximate.
- Ion impact energy at the target is estimated from `V_plasma - V_target`.
- Cu sputter yield uses a placeholder fit or future table.
- The model seeks a quasi-steady coupled solution, not a transient discharge.

