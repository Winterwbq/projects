# Cu PVD hybrid Zapdos input — B-guided nonlocal top-target SEE in Cartesian X-Z geometry.
# Lagged target-flux SEE feedback plus prescribed wafer RF boundary bias:
#   - 25 cm width by 30 cm height full plasma chamber (no symmetry plane)
#   - Cu-only chemistry
#   - original-density seeded quasi-neutral magnetron plasma
#   - magnetic electron transport enabled
#   - wafer/bottom chuck RF bias imposed directly as a Dirichlet plasma boundary
#     with a gentle startup ramp
#   - target Dirichlet potential is a 0 V plasma reference; target metal voltage
#     is kept outside Poisson and controls only the effective SEE energy scale
#   - an offline B-guided fast-electron spatial-weight table supplies source
#     shape; the computed, filtered target Cu+ flux supplies source amplitude
#   - mesh is refined near both electrodes for bulk transport gradients
#
# Coordinates:
#   x = physical X ∈ [0, chamber_width]
#   y = physical Z ∈ [0, chamber_height] (MOOSE names the second 2-D coordinate y)
#   wafer at Z=0 over X=7-17 cm
#   powered target/magnetron at Z=30 cm over X=19-22 cm
#
# Boundary layout:
#   left/right   — equal unresolved sheath-edge wall losses
#   bottom       — RF-biased wafer plus adjacent chuck-shield sections
#   top          — powered target plus a grounded shield section
#
# Reduced-model convention:
#   The computational target boundary is a prescribed plasma reference, not the
#   target metal. The target metal voltage remains outside the Poisson domain.
#   Its positive drop from the target plasma reference sets SEE energy
#   deposition. The computed target Bohm flux drives the lagged SEE feedback;
#   the 53 A value supplies only the existing discharge-power ceiling.
#   The wall metal voltage uses this same voltage reference, while plasma-side
#   sheath-edge potentials remain solved with natural Poisson flux.
#
#   The wafer and bottom shield use the requested RF waveform directly as their
#   plasma-potential Dirichlet value. This deliberately drives a bulk electric
#   field and does not predict either target or wafer sheath structure.
#
# Generate the common neutral map, the three B-field cases, and the SEE maps
# paired to those exact B-field tables:
#   python3 scripts/generate_cartesian_xz_neutral_cu.py
#   python3 scripts/generate_cartesian_xz_bfields.py --case all
#   python3 scripts/generate_cartesian_xz_see_maps.py --case all
#
# The default below selects the source-only B field and its paired source map.
# Invoke Zapdos from the cu_pvd_hybrid_2d/ root:
#   /Users/bingqingwang/projects/zapdos/zapdos-opt \
#     -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i
#
# Note: AddZapdosReactions (file_location) and PiecewiseMultilinear
# (data_file) resolve paths relative to the input file directory
# (zapdos_templates/), so all table paths use '../'.

chamber_height = 0.30
chamber_width  = 0.25
wafer_x_min    = 0.07
wafer_x_max    = 0.17
target_x_min   = 0.19
target_x_max   = 0.22
target_plasma_reference = 0
target_metal_voltage     = -300
wall_metal_voltage = 0
# Experimental total discharge current is used only to define a downward
# power ceiling. It no longer normalizes or boosts the computed Cu+ flux.
target_discharge_current = 53.0
target_max_discharge_power = ${fparse target_discharge_current * (target_plasma_reference - target_metal_voltage)}
wafer_dc_bias      = -75
wafer_rf_voltage   = 0
wafer_rf_frequency = 13.56e6
wafer_rf_ramp_time = 3.0e-7
gamma_secondary            = 0.10
secondary_electron_energy  = 10.0   # eV; Cu+ -> Cu potential emission gives roughly 1-10 eV
cu_ionization_energy       = 7.73
# The effective target drop is derived below from the plasma reference and
# target metal voltage; it is not solved by Poisson.
initial_seed_x_center     = 0.205
initial_seed_x_width      = 0.035
initial_seed_z_center     = 0.294
initial_seed_z_width      = 0.025
magnetron_initial_density_peak = 3e14
see_axial_decay_length = 0.012
see_target_radius = ${chamber_width}
see_target_edge_width = 0.010
see_radial_profile = annular_gaussian
see_target_radial_center = 0.205
see_target_radial_width = 0.035
see_normalize_annular_profile = true
# Provisional effective Cu ionization fraction for the unresolved fast-electron
# population. Calibrate after Ar/Ar+ and the discharge circuit are included.
see_ionization_efficiency = 0.25
see_max_ionizations_per_secondary = 12.0
see_bulk_energy_per_pair = 10
see_sheath_energy_absorption_fraction = 0.7
# The explicit fast-beam pair source consumes its own 7.73 eV per prescribed pair.
see_subtract_ionization_energy_from_absorbed_energy = true
# Reduced B-guided source coefficients. These reproduce the existing
# SEETargetIonFluxSource yield and energy bookkeeping for the present static
# target sheath voltage while replacing only its fixed spatial shape.
see_guided_ionization_yield = ${fparse min(see_ionization_efficiency*max((target_plasma_reference-target_metal_voltage)-cu_ionization_energy,0)/cu_ionization_energy,see_max_ionizations_per_secondary)}
see_guided_absorbed_energy = ${fparse secondary_electron_energy+see_sheath_energy_absorption_fraction*(target_plasma_reference-target_metal_voltage)}
# The inherited setting above is true, so the explicit ionization energy is
# subtracted before the remaining fast-electron energy thermalizes.
see_guided_thermalized_energy = ${fparse max(see_guided_absorbed_energy-see_guided_ionization_yield*cu_ionization_energy,0)}
see_feedback_response_time = 1e-7
density_ratio_floor = 1.0e12
plasma_neutral_runaway_ratio_limit = 1.0

electron_boundary_loss_ramp_time = 5.0e-8

# Wafer and bottom-shield Hagelaar losses retain their reflection controls.
# Both X boundaries are unresolved sheath edges with identical loss settings.
wafer_electron_reflection = 0.5
bottom_shield_electron_reflection = 0.5
side_wall_electron_reflection = 0.5
top_shield_electron_reflection = 0.5

wafer_ion_loss_scale = 0.5
bottom_shield_ion_loss_scale = 0.5


gas_temperature    = 300
cu_density_floor   = 1.0e16
cu_density_multiplier = 0.1

cu_uniform_background_density = 1.0e17
cu_magnetron_neutral_peak = 3.0e18
cu_magnetron_neutral_center_x = 0.205
cu_magnetron_neutral_center_y = 0.270
cu_magnetron_neutral_width_x = 0.080
cu_magnetron_neutral_width_y = 0.090

# Neutral background pressure used only in the electron transport pressure.
# Ar+ and Ar ionization chemistry are disabled in this Cu-only reduced model.
argon_background_pressure = 5  # Pa
k_boltzmann        = 1.380649e-23
avogadro           = 6.02214076e23

# Low-field reduced mobility for Cu+ in Ar from Yousef et al.,
# J. Chem. Phys. 127, 154309 (2007), Fig. 4: K0 ~ 2.2 cm^2/(V s).
cu_ion_reduced_mobility = 2.2e-4  # m^2/(V s)
cu_ion_reduced_mobility_reference_pressure = 101325
cu_ion_reduced_mobility_reference_temperature = 273.15

initial_plasma_floor = 1e14
initial_plasma_peak  = 0
initial_bulk_mean_energy = 6
magnetron_initial_mean_energy = 8.0
position_scale       = 1.0

table_dir      = '../runs/zapdos_cartesian_xz_25x30_neutral/moose_tables'
bfield_table_dir = '../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables'
cartesian_see_map_file = '../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/see_spatial_weight_m-1.tbl'
rate_coeff_dir = '../rate_coefficients_cu'

[GlobalParams]
  potential_units = V
  use_moles = true
[]

[Mesh]
  coord_type = XYZ

  [bottom_layer]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_width}
    ymin = 0
    ymax = 0.03
    nx = 120
    ny = 80
    # bias_y > 1 shrinks cells toward y=0, the wafer/RF electrode.
    # First wafer-adjacent cell is about 0.055 mm.
    bias_y = 1.04
    elem_type = QUAD4
  []

  [bulk_layer]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_width}
    ymin = 0.03
    ymax = 0.27
    nx = 120
    ny = 240
    elem_type = QUAD4
  []

  [top_layer]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_width}
    ymin = 0.27
    ymax = ${chamber_height}
    nx = 120
    ny = 80
    # bias_y < 1 shrinks cells toward ymax, the powered target.
    # Last target-adjacent cell is about 0.055 mm.
    bias_y = 0.96
    elem_type = QUAD4
  []

  [generated]
    type = StitchMeshGenerator
    inputs = 'bottom_layer bulk_layer top_layer'
    stitch_boundaries_pairs = 'top bottom; top bottom'
  []

  # Wafer segment at the bottom, X=7-17 cm.
  [wafer]
    type = ParsedGenerateSideset
    input = generated
    included_boundaries = bottom
    combinatorial_geometry = 'x >= ${wafer_x_min} & x <= ${wafer_x_max}'
    new_sideset_name = wafer
    enable_jit = false
  []

  [bottom_shield_left]
    type = ParsedGenerateSideset
    input = wafer
    included_boundaries = bottom
    combinatorial_geometry = 'x < ${wafer_x_min}'
    new_sideset_name = bottom_shield_left
    enable_jit = false
  []

  [bottom_shield_right]
    type = ParsedGenerateSideset
    input = bottom_shield_left
    included_boundaries = bottom
    combinatorial_geometry = 'x > ${wafer_x_max}'
    new_sideset_name = bottom_shield_right
    enable_jit = false
  []

  # Powered target at the top, X=19-22 cm.
  [target]
    type = ParsedGenerateSideset
    input = bottom_shield_right
    included_boundaries = top
    combinatorial_geometry = 'x >= ${target_x_min} & x <= ${target_x_max}'
    new_sideset_name = target
    enable_jit = false
  []

  [top_shield]
    type = ParsedGenerateSideset
    input = target
    included_boundaries = top
    combinatorial_geometry = 'x < ${target_x_min}'
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

    # Magnetized electron transport in the Cartesian X-Z plane. Zapdos keeps
    # legacy parameter names: magnetic_field_r is Bx and magnetic_field_z is Bz.
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
  [p_Cu_local]
  []
  # In-plane Cartesian X-Z B-field components (By stores physical Bz).
  [Bx_total]
  []
  [By_total]
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

[]

[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = target
    function = target_plasma_reference_func
    preset = false
  []

  # Prescribed RF plasma boundary used to drive the reduced bulk transport model.
  [wafer_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = wafer
    function = wafer_voltage_func
    preset = false
  []

  [bottom_shield_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = 'bottom_shield_left bottom_shield_right'
    function = wafer_voltage_func
    preset = false
  []

  [em_target]
    type = SheathLimitedElectronBC
    variable = em
    boundary = target
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = target_metal_voltage_func
    r = 0
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [em_wafer]
    type = HagelaarElectronBC
    variable = em
    boundary = wafer
    electron_energy = mean_en
    r = ${wafer_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [em_bottom_shield]
    type = HagelaarElectronBC
    variable = em
    boundary = 'bottom_shield_left bottom_shield_right'
    electron_energy = mean_en
    r = ${bottom_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [em_side_walls]
    type = SheathLimitedElectronBC
    variable = em
    boundary = 'left right'
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wall_metal_voltage_func
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [em_top_shield]
    type = SheathLimitedElectronBC
    variable = em
    boundary = top_shield
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wall_metal_voltage_func
    r = ${top_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_target]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = target
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    position_units = ${position_scale}
  []

  [Cup_wafer]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = wafer
    position_units = ${position_scale}
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_bottom_shield]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = 'bottom_shield_left bottom_shield_right'
    position_units = ${position_scale}
    loss_scale = ${bottom_shield_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_side_walls]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = 'left right'
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_top_shield]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = top_shield
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_target]
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = target
    electrons = em
    plasma_potential = potential
    electrode_potential = target_metal_voltage_func
    r = 0
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_wafer]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = wafer
    electrons = em
    r = ${wafer_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [mean_en_bottom_shield]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = 'bottom_shield_left bottom_shield_right'
    electrons = em
    r = ${bottom_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    clamp_actual_mean_energy = true
    actual_mean_energy_min = 0.01
    actual_mean_energy_max = 100
    use_magnetized_transport = true
    magnetic_field_r = Bx_total_func
    magnetic_field_z = By_total_func
  []

  [mean_en_side_walls]
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = 'left right'
    electrons = em
    plasma_potential = potential
    electrode_potential = wall_metal_voltage_func
    r = ${side_wall_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    energy_loss_coefficient = 1.666666666666667
  []

  [mean_en_top_shield]
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = top_shield
    electrons = em
    plasma_potential = potential
    electrode_potential = wall_metal_voltage_func
    r = ${top_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    energy_loss_coefficient = 1.666666666666667
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
  [target_plasma_reference_func]
    type = ParsedFunction
    expression = '${target_plasma_reference}'
  []

  [target_metal_voltage_func]
    type = ParsedFunction
    expression = '${target_metal_voltage}'
  []

  [wall_metal_voltage_func]
    type = ParsedFunction
    expression = '${wall_metal_voltage}'
  []

  [wafer_voltage_func]
    type = ParsedFunction
    expression = '${wafer_dc_bias} + tanh(t/${wafer_rf_ramp_time})*${wafer_rf_voltage}*sin(6.283185307179586*${wafer_rf_frequency}*t)'
  []

  [target_sheath_voltage_func]
    type = ParsedFunction
    # Positive target plasma-reference-to-metal drop. Making the target metal
    # more negative increases secondary energy/yield without moving the plasma
    # reference or multiplying the ion Bohm speed.
    expression = '0.5*((${target_plasma_reference}-${target_metal_voltage}) + sqrt((${target_plasma_reference}-${target_metal_voltage})^2))'
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

  [p_Cu_local_from_n_Cu_MC]
    type = ParsedFunction
    symbol_names = 'n'
    symbol_values = 'n_Cu_from_table'
    expression = '${argon_background_pressure} + ${k_boltzmann}*${gas_temperature}*(${cu_density_multiplier}*0.5*(n + ${cu_density_floor} + sqrt((n - ${cu_density_floor})^2)) + ${cu_uniform_background_density} + ${cu_magnetron_neutral_peak}*exp(-((x-${cu_magnetron_neutral_center_x})/${cu_magnetron_neutral_width_x})^2)*exp(-((y-${cu_magnetron_neutral_center_y})/${cu_magnetron_neutral_width_y})^2))'
  []

  # Cartesian X-Z B-field tables. By_total_func stores physical Bz because the
  # generated 2-D mesh uses MOOSE's internal x/y coordinate labels.
  [Bx_total_func]
    type = PiecewiseMultilinear
    data_file = '${bfield_table_dir}/Bx_T.tbl'
  []

  [By_total_func]
    type = PiecewiseMultilinear
    data_file = '${bfield_table_dir}/By_T.tbl'
  []

  [target_secondary_gamma]
    type = ParsedFunction
    expression = '${gamma_secondary}'
  []

  # Offline B-guided fast-electron deposition weight [m^-1]. Its Cartesian
  # volume integral equals eta_plasma times the target area; eta_plasma and the
  # case-specific f_local are already encoded in each generated table.
  [see_spatial_weight]
    type = PiecewiseMultilinear
    data_file = '${cartesian_see_map_file}'
  []

  # Multiplying by see_filtered_target_ion_flux [mol m^-2 s^-1] in the
  # ADBodyForce kernels gives balanced molar pair sources [mol m^-3 s^-1].
  [see_pair_source_coefficient]
    type = ParsedFunction
    symbol_names = 'W'
    symbol_values = 'see_spatial_weight'
    expression = '${gamma_secondary}*${see_guided_ionization_yield}*W'
  []

  # Electron energy source coefficient [eV m^-1].
  [see_energy_source_coefficient]
    type = ParsedFunction
    symbol_names = 'W'
    symbol_values = 'see_spatial_weight'
    expression = '${gamma_secondary}*${see_guided_thermalized_energy}*W'
  []

  [plasma_density_ic]
    type = ParsedFunction
    # Near-target seed centered on the B-field strength center at X=20.5 cm.
    expression = 'log((${initial_plasma_floor} + ${initial_plasma_peak}*256*(x/${chamber_width})^2*(1-x/${chamber_width})^2*(y/${chamber_height})^2*(1-y/${chamber_height})^2 + ${magnetron_initial_density_peak}*exp(-((x-${initial_seed_x_center})/${initial_seed_x_width})^2)*exp(-((y-${initial_seed_z_center})/${initial_seed_z_width})^2))/${avogadro})'
  []

  [mean_energy_ic]
    type = ParsedFunction
    expression = 'log(((${initial_plasma_floor} + ${initial_plasma_peak}*256*(x/${chamber_width})^2*(1-x/${chamber_width})^2*(y/${chamber_height})^2*(1-y/${chamber_height})^2)*${initial_bulk_mean_energy} + ${magnetron_initial_density_peak}*exp(-((x-${initial_seed_x_center})/${initial_seed_x_width})^2)*exp(-((y-${initial_seed_z_center})/${initial_seed_z_width})^2)*${magnetron_initial_mean_energy})/${avogadro})'
  []

  [potential_ic]
    type = ParsedFunction
    # Initial guess connects the prescribed wafer and target plasma boundaries.
    expression = '${wafer_dc_bias} + (${target_plasma_reference}-${wafer_dc_bias})*(y/${chamber_height})'
  []

[]

[Materials]
  [electron_transport]
    type = ElectronTransportCoefficients
    interp_trans_coeffs = true
    ramp_trans_coeffs = false
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
    heavy_species_p = p_Cu_local
    heavy_species_T = ${gas_temperature}
    reduced_mobility = ${cu_ion_reduced_mobility}
    reduced_mobility_reference_pressure = ${cu_ion_reduced_mobility_reference_pressure}
    reduced_mobility_reference_temperature = ${cu_ion_reduced_mobility_reference_temperature}
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
    boundary = target
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    flux_model = bohm
    use_moles = true
    secondary_emission = target_secondary_gamma
    position_units = ${position_scale}
    execution_order_group = -1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  # The source uses the previous accepted target state. The exact exponential
  # update represents unresolved ionization/ion-return/circuit response and is
  # independent of the numerical timestep.
  [see_target_flux_response]
    type = SEETargetIonFluxResponseUserObject
    target_ion_flux = target_cu_ion_flux
    sheath_voltage = target_sheath_voltage_func
    response_time = ${see_feedback_response_time}
    max_discharge_power = ${target_max_discharge_power}
    execution_order_group = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [neutral_inventory_guard]
    type = Terminator
    expression = 'cu_ion_to_neutral_max_ratio > ${plasma_neutral_runaway_ratio_limit} | electron_to_neutral_max_ratio > ${plasma_neutral_runaway_ratio_limit}'
    fail_mode = HARD
    error_level = ERROR
    message = 'Neutral inventory guard tripped: charged density is much larger than the fixed effective Cu neutral density. The fixed-neutral SEE/PVD approximation is no longer valid.'
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Kernels]
  # The lagged, power-capped target Cu+ Bohm flux supplies the time-dependent
  # amplitude. The offline table supplies the B-guided nonlocal spatial shape.
  [see_guided_electron_pair_source]
    type = ADBodyForce
    variable = em
    function = see_pair_source_coefficient
    postprocessor = see_filtered_target_ion_flux
    value = 1
    block = 0
  []

  [see_guided_cu_ion_pair_source]
    type = ADBodyForce
    variable = Cu+
    function = see_pair_source_coefficient
    postprocessor = see_filtered_target_ion_flux
    value = 1
    block = 0
  []

  [see_guided_electron_energy_source]
    type = ADBodyForce
    variable = mean_en
    function = see_energy_source_coefficient
    postprocessor = see_filtered_target_ion_flux
    value = 1
    block = 0
  []
[]

[Postprocessors]
  [target_potential_monitor]
    type = SideAverageValue
    variable = potential
    boundary = target
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_metal_voltage_monitor]
    type = FunctionValuePostprocessor
    function = target_metal_voltage_func
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_metal_voltage_monitor]
    type = FunctionValuePostprocessor
    function = wafer_voltage_func
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wall_metal_voltage_monitor]
    type = FunctionValuePostprocessor
    function = wall_metal_voltage_func
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_plasma_potential_min]
    type = SideExtremeValue
    variable = potential
    boundary = right
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_plasma_potential_average]
    type = SideAverageValue
    variable = potential
    boundary = right
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_plasma_potential_max]
    type = SideExtremeValue
    variable = potential
    boundary = right
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_sheath_drop_min]
    type = ParsedPostprocessor
    pp_names = 'right_wall_plasma_potential_min wall_metal_voltage_monitor'
    expression = 'right_wall_plasma_potential_min - wall_metal_voltage_monitor'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_sheath_drop_average]
    type = ParsedPostprocessor
    pp_names = 'right_wall_plasma_potential_average wall_metal_voltage_monitor'
    expression = 'right_wall_plasma_potential_average - wall_metal_voltage_monitor'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [right_wall_sheath_drop_max]
    type = ParsedPostprocessor
    pp_names = 'right_wall_plasma_potential_max wall_metal_voltage_monitor'
    expression = 'right_wall_plasma_potential_max - wall_metal_voltage_monitor'
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

  [neutral_cu_density_max]
    type = ElementExtremeValue
    variable = n_Cu_effective
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [cu_ion_to_neutral_max_ratio]
    type = ParsedPostprocessor
    pp_names = 'cu_ion_density_max neutral_cu_density_max'
    expression = 'cu_ion_density_max / max(neutral_cu_density_max, ${density_ratio_floor})'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_to_neutral_max_ratio]
    type = ParsedPostprocessor
    pp_names = 'electron_density_max neutral_cu_density_max'
    expression = 'electron_density_max / max(neutral_cu_density_max, ${density_ratio_floor})'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [neutral_availability_max_ratio]
    type = ParsedPostprocessor
    pp_names = 'neutral_cu_density_max cu_ion_density_max'
    expression = 'neutral_cu_density_max / (neutral_cu_density_max + cu_ion_density_max + ${density_ratio_floor})'
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

  [target_raw_bohm_current]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = raw_bohm_current
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_ion_current]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = ion_current
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_discharge_current]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = discharge_current
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_see_rate]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_cu_ion_flux
    value_type = secondary_electron_rate
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_raw_target_ion_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_target_flux_response
    value_type = raw_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_capped_target_ion_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_target_flux_response
    value_type = capped_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_filtered_target_ion_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_target_flux_response
    value_type = filtered_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_raw_discharge_power]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_target_flux_response
    value_type = raw_discharge_power
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_power_cap_factor]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_target_flux_response
    value_type = power_cap_factor
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_secondary_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    target_ion_flux_response = see_target_flux_response
    sheath_voltage = target_sheath_voltage_func
    secondary_emission = target_secondary_gamma
    spatial_weight_function = see_spatial_weight
    target_location = top
    axial_decay_length = ${see_axial_decay_length}
    target_radius = ${see_target_radius}
    target_edge_width = ${see_target_edge_width}
    radial_profile = ${see_radial_profile}
    target_radial_center = ${see_target_radial_center}
    target_radial_width = ${see_target_radial_width}
    normalize_annular_profile = ${see_normalize_annular_profile}
    ionization_energy = ${cu_ionization_energy}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_rate_relative_difference]
    # This is now a physical lag indicator, not a source-conservation error:
    # lagged SEE source rate versus instantaneous raw target SEE rate.
    type = RelativeDifferencePostprocessor
    value1 = see_secondary_rate_integral
    value2 = target_see_rate
    execution_order_group = 2
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_ionization_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = ionization_rate
    target_ion_flux_response = see_target_flux_response
    sheath_voltage = target_sheath_voltage_func
    secondary_emission = target_secondary_gamma
    spatial_weight_function = see_spatial_weight
    target_location = top
    axial_decay_length = ${see_axial_decay_length}
    target_radius = ${see_target_radius}
    target_edge_width = ${see_target_edge_width}
    radial_profile = ${see_radial_profile}
    target_radial_center = ${see_target_radial_center}
    target_radial_width = ${see_target_radial_width}
    normalize_annular_profile = ${see_normalize_annular_profile}
    ionization_energy = ${cu_ionization_energy}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_energy_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = electron_energy_rate
    target_ion_flux_response = see_target_flux_response
    sheath_voltage = target_sheath_voltage_func
    secondary_emission = target_secondary_gamma
    spatial_weight_function = see_spatial_weight
    target_location = top
    axial_decay_length = ${see_axial_decay_length}
    target_radius = ${see_target_radius}
    target_edge_width = ${see_target_edge_width}
    radial_profile = ${see_radial_profile}
    target_radial_center = ${see_target_radial_center}
    target_radial_width = ${see_target_radial_width}
    normalize_annular_profile = ${see_normalize_annular_profile}
    ionization_energy = ${cu_ionization_energy}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    bulk_energy_per_pair = ${see_bulk_energy_per_pair}
    emission_energy = ${secondary_electron_energy}
    sheath_energy_absorption_fraction = ${see_sheath_energy_absorption_fraction}
    subtract_ionization_energy_from_absorbed_energy = ${see_subtract_ionization_energy_from_absorbed_energy}
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_sheath_voltage_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = sheath_voltage
    target_ion_flux = target_cu_ion_flux
    use_target_flux_profile = true
    sheath_voltage = target_sheath_voltage_func
    secondary_emission = target_secondary_gamma
    target_location = top
    axial_decay_length = ${see_axial_decay_length}
    target_radius = ${see_target_radius}
    target_edge_width = ${see_target_edge_width}
    radial_profile = ${see_radial_profile}
    target_radial_center = ${see_target_radial_center}
    target_radial_width = ${see_target_radial_width}
    normalize_annular_profile = ${see_normalize_annular_profile}
    ionization_energy = ${cu_ionization_energy}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_ionization_yield_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = ionization_yield
    target_ion_flux = target_cu_ion_flux
    use_target_flux_profile = true
    sheath_voltage = target_sheath_voltage_func
    secondary_emission = target_secondary_gamma
    target_location = top
    axial_decay_length = ${see_axial_decay_length}
    target_radius = ${see_target_radius}
    target_edge_width = ${see_target_edge_width}
    radial_profile = ${see_radial_profile}
    target_radial_center = ${see_target_radial_center}
    target_radial_width = ${see_target_radial_width}
    normalize_annular_profile = ${see_normalize_annular_profile}
    ionization_energy = ${cu_ionization_energy}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
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

  [zapdos_plasma_state_sampler]
    type = NodalValueSampler
    variable = 'em mean_en Cu+'
    sort_by = id
    execute_on = 'FINAL'
  []

  [zapdos_neutral_state_sampler]
    type = NodalValueSampler
    variable = 'n_Cu_MC n_Cu_effective'
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
  end_time = 1.0e-3
  # Maintenance test: the initial condition already matches the applied bias.
  # Keep dt moderate so instability reflects plasma dynamics, not startup ramps.
  dtmax = 5e-6
  dtmin = 1.0e-13
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
      cutback_factor     = 0.5    # more aggressive cutback when Newton struggles
      dt                 = 1e-10
      growth_factor      = 1.7   # keep the source/sheath startup gradual
      optimal_iterations = 10      # target smaller accepted steps near stiff source growth
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
