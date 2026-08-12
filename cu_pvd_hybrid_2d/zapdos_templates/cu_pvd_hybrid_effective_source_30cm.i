# Cu PVD hybrid Zapdos input template — 2D Cartesian XY geometry.
# Reduced effective-source maintenance test:
#   - 30 cm chamber
#   - Cu-only chemistry
#   - pre-filled quasi-neutral magnetron plasma
#   - magnetic electron transport enabled
#   - wafer RF disabled for the first stability test
#   - prescribed magnetron-localized Cu ionization and electron energy sources
#
# Coordinates:
#   x ∈ [-chamber_radius, +chamber_radius]  (symmetric about chamber axis)
#   y ∈ [0, chamber_length]                 (target at y=0, wafer at y=L)
#
# Boundary layout:
#   bottom    — biased target electrode, treated as laterally infinite
#   top       — wafer (|x| ≤ wafer_radius) + top shield
#   left/right — artificial lateral truncation boundaries, not grounded walls
#
# Before running, generate MC fields and MOOSE tables (run from cu_pvd_hybrid_2d/ root):
#   python scripts/prepare_zapdos_inputs.py --out runs/zapdos_initial_input
#   python scripts/convert_mc_fields_to_moose_tables.py \
#     --mc-fields runs/zapdos_initial_input/mc_fields.csv \
#     --out-dir   runs/zapdos_initial_input/moose_tables
#
# Then invoke Zapdos also from the cu_pvd_hybrid_2d/ root so that table_dir
# ('runs/zapdos_initial_input/moose_tables') resolves correctly:
#   /path/to/zapdos-opt -i zapdos_templates/cu_pvd_hybrid_template.i
#
# Note: both AddZapdosReactions (file_location) and PiecewiseMultilinear
# (data_file) resolve paths relative to the input file directory
# (zapdos_templates/), so both table_dir and rate_coeff_dir use '../'.

chamber_length     = 0.3
chamber_radius     = 0.24
# Infinite-target approximation: the whole bottom boundary is powered target.
wafer_radius       = 0.15
target_voltage     = -300
# Boundary SEE is disabled in this reduced-source diagnostic. The prescribed
# effective source below represents the nonlocal result of SEE electrons after
# they leave the target, gyrate/drift, and ionize in the magnetron region.
gamma_secondary            = 0.0
secondary_electron_energy  = 10.0   # eV; Cu+→Cu potential emission gives ~1–10 eV, 8 eV is central
magnetron_radial_center        = 0.105
magnetron_radial_width         = 0.035
magnetron_axial_center         = 0.025
magnetron_axial_width          = 0.025
magnetron_initial_density_peak = 2.0e15
# Effective sources are read from S_Cu_eff_m3_s.tbl and Qe_eff_eV_m3_s.tbl.
# This ramp only controls the startup turn-on of those table-driven maps.
effective_source_ramp_time     = 1.0e-8
electron_boundary_loss_ramp_time = 1.0e-10

# Side/top/wafer boundaries are softened for the reduced maintenance test. The
# Hagelaar electron and energy losses scale as (1-r)/(1+r), so r=0.9 gives ~5%
# of the fully absorbing wall loss while keeping the biased target unchanged.
side_wall_electron_reflection = 0.995

gas_temperature    = 300
cu_density_floor   = 1.0e16
cu_density_multiplier = 0.1

cu_uniform_background_density = 3.0e18
cu_magnetron_neutral_peak = 4.0e18
cu_magnetron_neutral_center_x = ${magnetron_radial_center}
cu_magnetron_neutral_center_y = 0.050
cu_magnetron_neutral_width_x = 0.140
cu_magnetron_neutral_width_y = 0.120

# Neutral background pressure used only in the electron transport pressure.
# Ar+ and Ar ionization chemistry are disabled in this Cu-only reduced model.
argon_background_pressure = 10  # Pa
k_boltzmann        = 1.380649e-23
avogadro           = 6.02214076e23

initial_plasma_floor = 1e14
initial_plasma_peak  = 3e14
initial_bulk_mean_energy = 3.0
magnetron_initial_mean_energy = 8.0
position_scale       = 1.0

table_dir      = '../runs/zapdos_initial_input/moose_tables'
rate_coeff_dir = '../rate_coefficients_cu'

[GlobalParams]
  potential_units = V
  use_moles = true
[]

[Mesh]
  # 2D Cartesian — no coord_type (defaults to XY).
  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = ${fparse -chamber_radius}
    xmax = ${chamber_radius}
    ymin = 0
    ymax = ${chamber_length}
    nx = 160
    ny = 220
    bias_y = 1.02
    elem_type = QUAD4
  []

  # Wafer: top boundary, |x| < wafer_radius
  [wafer]
    type = ParsedGenerateSideset
    input = generated
    included_boundaries = top
    combinatorial_geometry = 'abs(x) < ${wafer_radius}'
    new_sideset_name = wafer
    enable_jit = false
  []

  # Top shield: top boundary outside wafer
  [top_shield]
    type = ParsedGenerateSideset
    input = wafer
    included_boundaries = top
    combinatorial_geometry = 'abs(x) >= ${wafer_radius}'
    new_sideset_name = top_shield
    enable_jit = false
  []
[]

[Problem]
  type = FEProblem
[]

[DriftDiffusionAction]
  [Plasma]
    electrons = em
    ions = 'Cu+'
    field = potential
    is_field_unique = true
    electron_energy = mean_en
    position_units = ${position_scale}
    additional_outputs = 'ElectronTemperature Current EField'

    # Magnetized electron transport — in-plane 2D Cartesian B-field.
    # The DriftDiffusionAction uses magnetic_field_r for the first spatial
    # coordinate (x here) and magnetic_field_z for the second (y here).
    use_magnetized_electron_transport = true
    use_magnetized_ion_transport = false
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []
[]

[Reactions]
  [Copper]
    species = 'em Cu+'
    aux_species = 'Cu'
    reaction_coefficient_format = 'rate'
    gas_species = 'Cu'
    electron_energy = mean_en
    electron_density = em
    include_electrons = true
    file_location = ${rate_coeff_dir}
    potential = potential
    use_log = true
    use_ad = true
    position_units = ${position_scale}
    block = 0
    reactions = 'em + Cu -> em + em + Cu+ : EEDF [-7.73] (reaction1)'
  []
[]

[AuxVariables]
  [Cu]
  []

  [n_Cu_MC]
  []
  [n_Cu_effective]
  []
  [S_iz_Cu_MC]
  []
  [p_Cu_local]
  []
  # In-plane Cartesian B-field components (Bz = 0)
  [Bx_total]
  []
  [By_total]
  []
  [S_Cu_eff]
  []
  [Qe_eff]
  []
[]

[AuxKernels]
  [Cu_background]
    type = FunctionAux
    variable = Cu
    function = cu_log_density_from_n_Cu_MC
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [n_Cu_from_MC]
    type = FunctionAux
    variable = n_Cu_MC
    function = n_Cu_from_table
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [n_Cu_effective_from_MC]
    type = FunctionAux
    variable = n_Cu_effective
    function = n_Cu_effective_from_n_Cu_MC
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [S_iz_Cu_from_MC]
    type = FunctionAux
    variable = S_iz_Cu_MC
    function = S_iz_Cu_from_table
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [p_Cu_local_from_MC]
    type = FunctionAux
    variable = p_Cu_local
    function = p_Cu_local_from_n_Cu_MC
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [Bx_from_MC]
    type = FunctionAux
    variable = Bx_total
    function = Bx_total_func
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [By_from_MC]
    type = FunctionAux
    variable = By_total
    function = By_total_func
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [S_Cu_eff_aux]
    type = FunctionAux
    variable = S_Cu_eff
    function = effective_cu_ionization_source_density
    execute_on = 'INITIAL TIMESTEP_BEGIN TIMESTEP_END'
  []

  [Qe_eff_aux]
    type = FunctionAux
    variable = Qe_eff
    function = effective_electron_energy_source_density
    execute_on = 'INITIAL TIMESTEP_BEGIN TIMESTEP_END'
  []
[]

[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = bottom
    function = target_voltage_func
    preset = false
  []

  # Grounded physical top metal boundaries for the maintenance test. The left
  # and right boundaries are artificial lateral truncation boundaries and are
  # intentionally left without grounded-wall BCs.
  [wafer_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = wafer
    function = wafer_voltage_func
    preset = false
  []

  [wall_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = top_shield
    function = side_wall_voltage_func
    preset = false
  []

  [em_target]
    type = HagelaarElectronBC
    variable = em
    boundary = bottom
    electron_energy = mean_en
    r = 0
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [target_secondary_electrons]
    type = SecondaryElectronBC
    variable = em
    boundary = bottom
    ions = 'Cu+'
    electron_energy = mean_en
    r = 0
    emission_coeffs = 'gamma_target'
    position_units = ${position_scale}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
    emission_drift_floor_fraction = 0.1
  []

  [em_wafer]
    type = HagelaarElectronBC
    variable = em
    boundary = wafer
    electron_energy = mean_en
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [em_wall]
    type = HagelaarElectronBC
    variable = em
    boundary = top_shield
    electron_energy = mean_en
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [Cup_target]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = bottom
    position_units = ${position_scale}
  []

  [Cup_wafer]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = wafer
    position_units = ${position_scale}
    loss_scale = 0.01
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_wall]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = top_shield
    position_units = ${position_scale}
    loss_scale = 0.05
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_target]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = bottom
    electrons = em
    r = 0
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [target_secondary_electron_energy]
    type = SecondaryElectronEnergyBC
    variable = mean_en
    boundary = bottom
    electrons = em
    ions = 'Cu+'
    r = 0
    emission_coeffs = 'gamma_target'
    secondary_electron_energy = ${secondary_electron_energy}
    position_units = ${position_scale}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
    emission_drift_floor_fraction = 0.1
  []

  [mean_en_wafer]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = wafer
    electrons = em
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [mean_en_wall]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = top_shield
    electrons = em
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []
[]

[ICs]
  # AuxVariable ICs — ensure Materials never see zero before FunctionAux runs.
  # p_Cu_local is the effective neutral pressure used by electron transport.
  # It includes residual Ar background pressure plus the MC Cu vapor pressure.
  [p_Cu_local_ic]
    type = ConstantIC
    variable = p_Cu_local
    value = ${argon_background_pressure}
  []

  # Cu: log-molar density at the floor plus the broad background reservoir.
  [Cu_ic]
    type = ConstantIC
    variable = Cu
    value = ${fparse log((cu_density_multiplier * cu_density_floor + cu_uniform_background_density) / avogadro)}
  []

  [em_ic]
    type = FunctionIC
    variable = em
    function = plasma_density_ic
  []

  [Cup_ic]
    type = FunctionIC
    variable = Cu+
    function = plasma_density_ic
  []

  [mean_en_ic]
    type = FunctionIC
    variable = mean_en
    function = mean_energy_ic
  []

  [potential_ic]
    type = FunctionIC
    variable = potential
    function = potential_ic
  []
[]

[Functions]
  [target_voltage_func]
    type = ParsedFunction
    # Flat voltage over the powered target. The grounded guard strip is now only
    # a tiny corner feature; a smooth low-voltage target margin created artificial
    # ion depletion/spikes near |x| ~= target_radius.
    expression = '${target_voltage}'
  []

  [wafer_voltage_func]
    type = ParsedFunction
    expression = '0'
  []

  [side_wall_voltage_func]
    type = ParsedFunction
    expression = '0'
  []

  [cu_log_density_from_n_Cu_MC]
    type = ParsedFunction
    symbol_names = 'n'
    symbol_values = 'n_Cu_from_table'
    expression = 'log((${cu_density_multiplier}*0.5*(n + ${cu_density_floor} + sqrt((n - ${cu_density_floor})^2)) + ${cu_uniform_background_density} + ${cu_magnetron_neutral_peak}*exp(-((x-${cu_magnetron_neutral_center_x})/${cu_magnetron_neutral_width_x})^2)*exp(-((y-${cu_magnetron_neutral_center_y})/${cu_magnetron_neutral_width_y})^2))/${avogadro})'
  []

  [n_Cu_effective_from_n_Cu_MC]
    type = ParsedFunction
    symbol_names = 'n'
    symbol_values = 'n_Cu_from_table'
    expression = '${cu_density_multiplier}*0.5*(n + ${cu_density_floor} + sqrt((n - ${cu_density_floor})^2)) + ${cu_uniform_background_density} + ${cu_magnetron_neutral_peak}*exp(-((x-${cu_magnetron_neutral_center_x})/${cu_magnetron_neutral_width_x})^2)*exp(-((y-${cu_magnetron_neutral_center_y})/${cu_magnetron_neutral_width_y})^2)'
  []

  [n_Cu_from_table]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/n_Cu_m3.tbl'
  []

  [S_iz_Cu_from_table]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/S_iz_Cu_m3_s.tbl'
  []

  [p_Cu_local_from_n_Cu_MC]
    type = ParsedFunction
    symbol_names = 'n'
    symbol_values = 'n_Cu_from_table'
    expression = '${argon_background_pressure} + ${k_boltzmann}*${gas_temperature}*(${cu_density_multiplier}*0.5*(n + ${cu_density_floor} + sqrt((n - ${cu_density_floor})^2)) + ${cu_uniform_background_density} + ${cu_magnetron_neutral_peak}*exp(-((x-${cu_magnetron_neutral_center_x})/${cu_magnetron_neutral_width_x})^2)*exp(-((y-${cu_magnetron_neutral_center_y})/${cu_magnetron_neutral_width_y})^2))'
  []

  # In-plane Cartesian B-field tables (no Bz/Btheta)
  [Bx_total_func]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/Bx_T.tbl'
  []

  [By_total_func]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/By_T.tbl'
  []

  [target_secondary_gamma]
    type = ParsedFunction
    expression = '${gamma_secondary}'
  []

  [plasma_density_ic]
    type = ParsedFunction
    # Broad seed plus a one-sided near-target magnetron seed at x = +magnetron_radial_center.
    expression = 'log((${initial_plasma_floor} + ${initial_plasma_peak}*(1-(x/${chamber_radius})^2)^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2 + ${magnetron_initial_density_peak}*exp(-((x-${magnetron_radial_center})/${magnetron_radial_width})^2)*exp(-((y-${magnetron_axial_center})/${magnetron_axial_width})^2))/${avogadro})'
  []

  [mean_energy_ic]
    type = ParsedFunction
    expression = 'log(((${initial_plasma_floor} + ${initial_plasma_peak}*(1-(x/${chamber_radius})^2)^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2)*${initial_bulk_mean_energy} + ${magnetron_initial_density_peak}*exp(-((x-${magnetron_radial_center})/${magnetron_radial_width})^2)*exp(-((y-${magnetron_axial_center})/${magnetron_axial_width})^2)*${magnetron_initial_mean_energy})/${avogadro})'
  []

  [effective_cu_ionization_source_density]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/S_Cu_eff_m3_s.tbl'
  []

  [effective_cu_ionization_source_molar]
    type = ParsedFunction
    # Kernel source for log-molar Zapdos variables. The table stores the
    # physical source in m^-3 s^-1; this ramp only controls startup.
    symbol_names = 'S'
    symbol_values = 'effective_cu_ionization_source_density'
    expression = '(S/${avogadro})*tanh(t/${effective_source_ramp_time})'
  []

  [effective_electron_energy_source_density]
    type = PiecewiseMultilinear
    data_file = '${table_dir}/Qe_eff_eV_m3_s.tbl'
  []

  [effective_electron_energy_source_molar]
    type = ParsedFunction
    # Kernel source for mean_en = log(electron energy density / Avogadro).
    symbol_names = 'Q'
    symbol_values = 'effective_electron_energy_source_density'
    expression = '(Q/${avogadro})*tanh(t/${effective_source_ramp_time})'
  []

  [potential_ic]
    type = ParsedFunction
    # Initial guess for a large powered target and grounded top wall.
    expression = '${target_voltage}*(1-y/${chamber_length})'
  []

[]

[Materials]
  [electron_transport]
    type = ElectronTransportCoefficients
    interp_trans_coeffs = true
    ramp_trans_coeffs = true
    pressure_dependent_electron_coeff = true
    p_gas = p_Cu_local
    electrons = em
    electron_energy = mean_en
    clamp_actual_mean_energy = true
    property_tables_file = ${rate_coeff_dir}/electron_moments.txt
  []

  [gas_permittivity]
    type = ElectrostaticPermittivity
    potential = potential
  []

  [cu_ion]
    type = ADHeavySpecies
    heavy_species_name = Cu+
    heavy_species_mass = 1.0552069e-25
    heavy_species_charge = 1.0
    mobility = 54.4
    diffusivity = 1.5
  []

  [cu_neutral]
    type = ADHeavySpecies
    heavy_species_name = Cu
    heavy_species_mass = 1.0552069e-25
    heavy_species_charge = 0.0
  []

  [secondary_emission_coefficients]
    type = ADGenericFunctionMaterial
    block = 0
    prop_names = 'gamma_target'
    prop_values = 'target_secondary_gamma'
  []
[]

[UserObjects]
  [target_cu_ion_flux]
    type = TargetIonFluxSideUserObject
    boundary = bottom
    ions = Cu+
    ion_temperature = 300
    flux_model = secondary_electron_bc
    r_ion = 0
    position_units = ${position_scale}
    execute_on = 'INITIAL LINEAR TIMESTEP_END'
  []

[]

[Kernels]
  # Reduced effective source: prescribed nonlocal result of hot SEE electrons.
  # It creates balanced electron/Cu+ pairs and deposits electron energy in the
  # same magnetron-localized region. The ordinary Cu EEDF reaction remains
  # active, so this can be reduced or removed after a self-sustaining state is
  # found.
  [effective_electron_pair_source]
    type = ADBodyForce
    variable = em
    function = effective_cu_ionization_source_molar
    value = 1
    block = 0
  []

  [effective_cu_ion_pair_source]
    type = ADBodyForce
    variable = Cu+
    function = effective_cu_ionization_source_molar
    value = 1
    block = 0
  []

  [effective_electron_energy_source]
    type = ADBodyForce
    variable = mean_en
    function = effective_electron_energy_source_molar
    value = 1
    block = 0
  []
[]

[Postprocessors]
  [target_voltage_monitor]
    type = SideAverageValue
    variable = potential
    boundary = bottom
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_voltage_monitor]
    type = SideAverageValue
    variable = potential
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_temperature_average]
    type = ElementAverageValue
    variable = e_temp
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_temperature_max]
    type = ElementExtremeValue
    variable = e_temp
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_density_max]
    type = ElementExtremeValue
    variable = em_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_density_integral]
    type = ElementIntegralVariablePostprocessor
    variable = em_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [cu_ion_density_max]
    type = ElementExtremeValue
    variable = Cu+_density
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [cu_ion_density_integral]
    type = ElementIntegralVariablePostprocessor
    variable = Cu+_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_ion_flux_average]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = average
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_ion_flux_integral]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = integral
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [potential_min]
    type = NodalExtremeValue
    variable = potential
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [potential_max]
    type = NodalExtremeValue
    variable = potential
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[VectorPostprocessors]
  [zapdos_potential_sampler]
    type = NodalValueSampler
    variable = potential
    sort_by = id
    execute_on = 'FINAL'
  []

  [zapdos_element_field_sampler]
    type = ElementValueSampler
    variable = 'em_density Cu+_density e_temp EFieldx EFieldy'
    sort_by = id
    execute_on = 'FINAL'
  []

  [zapdos_energy_sampler]
    type = NodalValueSampler
    variable = 'em mean_en Cu+ n_Cu_MC n_Cu_effective S_Cu_eff Qe_eff'
    sort_by = id
    execute_on = 'FINAL'
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  automatic_scaling = true
  compute_scaling_once = false
  end_time = 1.0e-6
  # Maintenance test: the initial condition already matches the applied bias.
  # Keep dt moderate so instability reflects plasma dynamics, not startup ramps.
  dtmax = 2.0e-9
  dtmin = 1.0e-16
  scheme = implicit-euler
  solve_type = NEWTON
  line_search = 'bt'

  petsc_options = '-snes_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type'
  petsc_options_value  = 'lu NONZERO 1.e-6 preonly'

  nl_rel_tol = 1e-4   # loosened for startup; tighten once plasma is established
  nl_abs_tol = 1e-14  # effectively disabled — rely on relative tolerance
  l_max_its  = 15
  nl_max_its = 15

  [TimeSteppers]
    [Adaptive]
      type = IterationAdaptiveDT
      cutback_factor     = 0.4    # more aggressive cutback when Newton struggles
      dt                 = 1.0e-12
      growth_factor      = 1.15   # keep the table-driven source turn-on gradual
      optimal_iterations = 8      # target smaller accepted steps near stiff source growth
    []
  []
[]

[Outputs]
  perf_graph = true
  csv = false
  [exodus]
    type = Exodus
    time_step_interval = 2
  []
[]
