# How to Write the Zapdos `.i` Input File

Zapdos uses a MOOSE input file, usually named `something.i`. The `.i` file tells Zapdos:

```text
1. What mesh to solve on
2. What unknown variables to solve
3. What equations/kernels/actions to use
4. What boundary conditions to apply
5. What material/transport coefficients to use
6. What functions or tables to read from MC
7. What outputs to write back to MC
```

For the Cu PVD hybrid model, the MC side provides:

```text
n_Cu(r,z)
S_iz_Cu(r,z)
B_total(r,z)
boundary flux estimates
target source profile
```

The current Zapdos template is now set up as a Cu+ dominant plasma template:

```text
electrons = em
ions = Cu+
neutral Cu field = n_Cu from MC
Cu ionization source = S_iz_Cu from MC, scaled by cu_source_scale
```

For the Cu-dominant run, the neutral background should be interpreted as Cu
atoms from MC, not residual Ar gas. The template therefore constructs a local Cu
vapor pressure from the MC density field,

```text
p_Cu_local(r,z) = max(n_Cu_MC(r,z), n_floor) k_B T_gas
```

and couples that AuxVariable into Zapdos as `p_gas = p_Cu_local` for
`ElectronTransportCoefficients`. The density floor prevents near-empty MC cells
from producing unrealistically large electron mobility and diffusion.

The MC/coupler writes `S_iz_Cu` in particle units, `m^-3 s^-1`. Because this
Zapdos input uses `use_moles = true`, the source is converted inside the input
file before it is applied:

```text
S_iz_Zapdos = cu_source_scale * S_iz_MC / N_A
```

so the source entering the log-density continuity equations is in
`mol m^-3 s^-1`.

The practical Zapdos input-file strategy is:

```text
MC CSV fields
  -> convert to MOOSE PiecewiseMultilinear tables
  -> load them in [Functions]
  -> copy them into [AuxVariables] using [AuxKernels]
  -> use Bz_total in DriftDiffusionAction
  -> use S_iz_Cu as a source for electrons and Cu+
  -> output phi, E, ne, Te for the MC Cu+ pusher
```

## Files I Prepared

Template Zapdos input file:

```text
zapdos_templates/cu_pvd_hybrid_template.i
```

Converter from MC CSV to MOOSE tables:

```text
scripts/convert_mc_fields_to_moose_tables.py
```

## Step 1: Generate the MC-to-Zapdos Data

```bash
cd /Users/bingqingwang/projects/cu_pvd_hybrid
python scripts/prepare_zapdos_inputs.py --out runs/zapdos_initial_input
```

This creates:

```text
runs/zapdos_initial_input/mc_fields.csv
runs/zapdos_initial_input/boundary_fluxes.csv
runs/zapdos_initial_input/target_source.csv
runs/zapdos_initial_input/operation.csv
```

## Step 2: Convert `mc_fields.csv` to MOOSE Tables

```bash
python scripts/convert_mc_fields_to_moose_tables.py \
  --mc-fields runs/zapdos_initial_input/mc_fields.csv \
  --out-dir runs/zapdos_initial_input/moose_tables
```

This creates:

```text
n_Cu_m3.tbl
S_iz_Cu_m3_s.tbl
Br_T.tbl
Btheta_T.tbl
Bz_T.tbl
Bmag_T.tbl
```

Each table has the format required by MOOSE `PiecewiseMultilinear`:

```text
AXIS X
r0 r1 r2 ...
AXIS Y
z0 z1 z2 ...
DATA
field values with r-index changing fastest
```

## Step 3: Write the Zapdos `.i` File

The most important blocks are below.

### Mesh

Use the same coordinate convention as the MC:

```text
[Mesh]
  coord_type = RZ
  rz_coord_axis = Y

  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.24
    ymin = 0
    ymax = 0.60
    nx = 64
    ny = 96
    elem_type = QUAD4
  []
[]
```

Here:

```text
x = r
y = z
bottom = target
top = wafer
right = chamber wall
```

### Plasma Equations

Use Zapdos `DriftDiffusionAction` with Cu+ as the ion:

```text
[DriftDiffusionAction]
  [Plasma]
    electrons = em
    ions = Cu+
    field = potential
    electron_energy = mean_en
    additional_outputs = 'ElectronTemperature Current EField'
    use_magnetized_electron_transport = true
    magnetic_field_z = Bz_total
  []
[]
```

This tells Zapdos to solve:

```text
electron density
Cu+ ion density
potential
mean electron energy
```

and to use the MC-provided `Bz_total` function for magnetized electron transport.

### Load MC Fields

In `[Functions]`, load the MC fields:

```text
[Functions]
  [n_Cu_from_table]
    type = PiecewiseMultilinear
    data_file = 'runs/zapdos_initial_input/moose_tables/n_Cu_m3.tbl'
  []

  [S_iz_Cu_from_table]
    type = PiecewiseMultilinear
    data_file = 'runs/zapdos_initial_input/moose_tables/S_iz_Cu_m3_s.tbl'
  []

  [Bz_total]
    type = PiecewiseMultilinear
    data_file = 'runs/zapdos_initial_input/moose_tables/Bz_T.tbl'
  []
[]
```

Then expose them as output variables:

```text
[AuxVariables]
  [n_Cu_MC]
  []
  [S_iz_Cu_MC]
  []
  [Bz_total_aux]
  []
[]

[AuxKernels]
  [n_Cu_from_MC]
    type = FunctionAux
    variable = n_Cu_MC
    function = n_Cu_from_table
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [S_iz_Cu_from_MC]
    type = FunctionAux
    variable = S_iz_Cu_MC
    function = S_iz_Cu_from_table
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [Bz_from_MC]
    type = FunctionAux
    variable = Bz_total_aux
    function = Bz_total
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []
[]
```

### Boundary Conditions

For this chamber:

```text
bottom = target
top = wafer
right = wall
```

Potential BCs:

```text
[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = bottom
    function = target_voltage_func
  []

  [wafer_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = top
    function = wafer_voltage_func
  []

  [wall_potential]
    type = DirichletBC
    variable = potential
    boundary = right
    value = 0
  []
[]
```

For RF wafer bias, define:

```text
wafer_rf_amplitude = 50
wafer_rf_frequency = 13.56e6
wafer_rf_ramp_time = 2.0e-7
```

and use:

```text
[Functions]
  [wafer_voltage_func]
    type = ParsedFunction
    expression = '${wafer_rf_amplitude}*tanh(t/${wafer_rf_ramp_time})*sin(2*pi*${wafer_rf_frequency}*t)'
  []
[]
```

Electron and ion wall-loss BCs are then added using Zapdos BCs such as:

```text
HagelaarElectronBC
LymberopoulosElectronBC
LymberopoulosIonBC
ElectronTemperatureDirichletBC
```

The template already includes these.

## What Values You Need to Put Into Zapdos

From the MC/coupling side:

```text
n_Cu_m3              neutral Cu density [m^-3]
S_iz_Cu_m3_s         Cu ionization source [m^-3 s^-1]
Bz_T                 axial B_total [T]
Br_T, Btheta_T       optional, useful for diagnostics or future full-vector coupling
target_bias_V        target voltage [V]
pressure_Ar_Torr     recorded process setting only; not used for Cu transport
target/wafer/wall BC choices
```

From plasma/chemistry assumptions:

```text
electron transport coefficient table
local Cu pressure p_Cu_local = max(n_Cu_MC, n_floor) k_B T_gas
Cu reaction/source model
initial electron density
initial ion density
initial mean electron energy
secondary electron emission coefficient
electron temperature boundary value
```

## Important Note About `S_iz_Cu`

`S_iz_Cu` is now connected to the plasma equations using `BodyForce` source terms:

```text
[Kernels]
  [em_cu_ionization_source]
    type = BodyForce
    variable = em
    function = S_iz_Cu_scaled
  []

  [Cup_cu_ionization_source]
    type = BodyForce
    variable = Cu+
    function = S_iz_Cu_scaled
  []
[]
```

The scale factor is:

```text
cu_source_scale = 1.0e-30
```

This is deliberately tiny for startup. Increase it gradually during continuation. Directly applying the full MC source can make the nonlinear solve diverge.

Important limitation: the template still uses the existing Zapdos electron transport coefficient table path. For production Cu plasma, replace this with Cu electron transport and Cu ionization/energy-loss data.

The template also includes:

```text
[VectorPostprocessors/zapdos_potential_sampler]
[VectorPostprocessors/zapdos_element_field_sampler]
```

which samples these Zapdos fields at the final time:

```text
potential                  nodal sample
em_density, e_temp          element samples
EFieldx, EFieldy            element samples
```

Those sampled values are what the MC side needs back in this format:

```text
r,z,phi,ne,Te,Er,Ez
```

## How to Run

Run with the Zapdos executable and the template path. The exact executable name depends on your build, for example:

```bash
cd /Users/bingqingwang/projects/cu_pvd_hybrid
./zapdos-opt -i /Users/bingqingwang/projects/cu_pvd_hybrid/zapdos_templates/cu_pvd_hybrid_template.i
```

For your current workspace the executable appears to be:

```bash
/Users/bingqingwang/projects/zapdos/zapdos-opt -i /Users/bingqingwang/projects/cu_pvd_hybrid/zapdos_templates/cu_pvd_hybrid_template.i
```

If your executable is under a different build name, use that instead, for example `zapdos-dbg`.
