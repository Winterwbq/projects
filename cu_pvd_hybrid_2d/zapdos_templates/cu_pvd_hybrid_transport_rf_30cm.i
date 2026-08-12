# Cu PVD hybrid Zapdos input template - transport/RF study in 2D axisymmetric R-Z geometry.
# Already-generated-plasma transport diagnostic:
#   - 30 cm radius/height chamber surrogate
#   - Cu-only chemistry
#   - plasma state initialized from the last saved new_BC Zapdos solution
#   - magnetic electron transport enabled
#   - wafer/bottom chuck RF bias enabled with a gentle startup ramp
#   - fixed effective ionization/energy source maps are removed
#   - subsequent Cu ionization comes from the local EEDF reaction block
#
# Coordinates:
#   x = r ∈ [0, chamber_radius]
#   y = z ∈ [0, chamber_length]
#   wafer at z=0, powered target/magnetron at z=L
#
# Boundary layout:
#   left         — axis of symmetry
#   right        — floating outer wall with particle/energy losses
#   bottom       — RF-biased wafer disk plus adjacent chuck shield
#   top          — powered startup target across the whole top boundary
#
# Before running, make sure the source state Exodus file exists:
#   zapdos_templates/cu_pvd_hybrid_hpem_rz_30cm_new_BC.e
#
# Also generate the R-Z analytic HPEM-like tables for neutral Cu and B-field:
#   python scripts/generate_hpem_rz_30cm_tables.py
#
# Then invoke Zapdos from the cu_pvd_hybrid_2d/ root:
#   /Users/bingqingwang/projects/zapdos/zapdos-opt \
#     -i zapdos_templates/cu_pvd_hybrid_transport_rf_30cm.i
#
# Note: both AddZapdosReactions (file_location) and PiecewiseMultilinear
# (data_file) resolve paths relative to the input file directory
# (zapdos_templates/), so both table_dir and rate_coeff_dir use '../'.

chamber_length     = 0.3
chamber_radius     = 0.30
wafer_radius       = 0.15
target_voltage     = -300
wafer_dc_bias      = -5
wafer_rf_voltage   = 20
wafer_rf_frequency = 13.56e6
wafer_rf_ramp_time = 3.0e-7
# Weak target secondary electron emission provides a transport-coupled
# maintenance mechanism: electron supply scales with the Cu+ flux reaching the
# magnetron target instead of a prescribed volumetric source map.
gamma_secondary            = 0.08
secondary_electron_energy  = 10.0   # eV; Cu+ -> Cu potential emission gives roughly 1-10 eV
magnetron_radial_center        = 0.235
electron_boundary_loss_ramp_time = 3.0e-7

# Boundary particle-loss controls. Hagelaar electron/energy losses scale as
# (1-r)/(1+r): r=0 is fully absorbing and r=0.9999 is almost reflective.
# The RF sheath is not resolved, so these are sheath-averaged effective losses
# at the computational boundary rather than raw metal-surface collection.
wafer_electron_reflection = 0.8
bottom_shield_electron_reflection = 0.8
right_wall_electron_reflection = 0.7

wafer_ion_loss_scale = 0.01
bottom_shield_ion_loss_scale = 0.01
right_wall_ion_loss_scale = 0.5


gas_temperature    = 300
cu_density_floor   = 1.0e16
cu_density_multiplier = 0.1

cu_uniform_background_density = 1.0e17
cu_magnetron_neutral_peak = 3.0e18
cu_magnetron_neutral_center_x = ${magnetron_radial_center}
cu_magnetron_neutral_center_y = 0.245
cu_magnetron_neutral_width_x = 0.080
cu_magnetron_neutral_width_y = 0.090

# Neutral background pressure used only in the electron transport pressure.
# Ar+ and Ar ionization chemistry are disabled in this Cu-only reduced model.
argon_background_pressure = 10  # Pa
k_boltzmann        = 1.380649e-23
avogadro           = 6.02214076e23

position_scale       = 1.0

source_plasma_state_file = 'cu_pvd_hybrid_hpem_rz_30cm_new_BC.e'
table_dir      = '../runs/zapdos_hpem_rz_30cm/moose_tables'
rate_coeff_dir = '../rate_coefficients_cu'

[GlobalParams]
  potential_units = V
  use_moles = true
[]

[Mesh]
  coord_type = RZ
  rz_coord_axis = Y
  # SolutionUserObject samples an Exodus state file during IC setup. Keep the
  # mesh replicated so that source-state interpolation is deterministic.
  parallel_type = replicated

  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_radius}
    ymin = 0
    ymax = ${chamber_length}
    nx = 120
    ny = 160
    bias_y = 0.98
    elem_type = QUAD4
  []

  # Wafer disk at the bottom.
  [wafer]
    type = ParsedGenerateSideset
    input = generated
    included_boundaries = bottom
    combinatorial_geometry = 'x <= ${wafer_radius}'
    new_sideset_name = wafer
    enable_jit = false
  []

  [bottom_shield]
    type = ParsedGenerateSideset
    input = wafer
    included_boundaries = bottom
    combinatorial_geometry = 'x > ${wafer_radius}'
    new_sideset_name = bottom_shield
    enable_jit = false
  []

  # Startup target: power the whole top boundary to avoid target/shield edge
  # singularities while debugging bulk plasma formation.
  [target]
    type = ParsedGenerateSideset
    input = bottom_shield
    included_boundaries = top
    combinatorial_geometry = 'x >= 0'
    new_sideset_name = target
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

    # Magnetized electron transport — in-plane R-Z B-field.
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
  [p_Cu_local]
  []
  # In-plane R-Z B-field components.
  [Bx_total]
  []
  [By_total]
  []

  # Wafer RF sheath diagnostics. These are not coupled back into the plasma
  # equations; they translate the sheath-edge solution into estimated wafer
  # bombardment quantities.
  [wafer_sheath_voltage]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_sheath_thickness]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_ion_flux_sheath]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_ion_current_density]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_ion_impact_energy]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_ion_transit_time]
    family = MONOMIAL
    order = CONSTANT
  []
  [wafer_rf_period_fraction]
    family = MONOMIAL
    order = CONSTANT
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

  [wafer_sheath_voltage_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_sheath_voltage
    boundary = wafer
    quantity = sheath_voltage
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_sheath_thickness_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_sheath_thickness
    boundary = wafer
    quantity = sheath_thickness
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_flux_sheath_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_ion_flux_sheath
    boundary = wafer
    quantity = ion_flux
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_current_density_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_ion_current_density
    boundary = wafer
    quantity = ion_current_density
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_impact_energy_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_ion_impact_energy
    boundary = wafer
    quantity = ion_impact_energy
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_transit_time_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_ion_transit_time
    boundary = wafer
    quantity = ion_transit_time
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_rf_period_fraction_aux]
    type = RFSheathDiagnosticsAux
    variable = wafer_rf_period_fraction
    boundary = wafer
    quantity = rf_period_fraction
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    position_units = ${position_scale}
    use_moles = true
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
    ion_temperature = ${gas_temperature}
    rf_frequency = ${wafer_rf_frequency}
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = target
    function = target_voltage_func
    preset = false
  []

  # Wafer/chuck RF is now treated as a metal voltage behind a sheath-edge
  # boundary. Do not impose the RF voltage directly on the plasma potential;
  # otherwise the first mesh row must carry the full unresolved sheath drop.

  [em_target]
    type = HagelaarElectronBC
    variable = em
    boundary = target
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
    boundary = target
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
    type = SheathLimitedElectronBC
    variable = em
    boundary = wafer
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    r = ${wafer_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [em_bottom_shield]
    type = SheathLimitedElectronBC
    variable = em
    boundary = bottom_shield
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    r = ${bottom_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [em_right_wall]
    type = HagelaarElectronBC
    variable = em
    boundary = right
    electron_energy = mean_en
    r = ${right_wall_electron_reflection}
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
    boundary = target
    position_units = ${position_scale}
  []

  [Cup_wafer]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = wafer
    electrons = em
    electron_energy = mean_en
    ion_temperature = ${gas_temperature}
    position_units = ${position_scale}
    loss_scale = ${wafer_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_bottom_shield]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = bottom_shield
    electrons = em
    electron_energy = mean_en
    ion_temperature = ${gas_temperature}
    position_units = ${position_scale}
    loss_scale = ${bottom_shield_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [Cup_right_wall]
    type = LymberopoulosIonBC
    variable = Cu+
    boundary = right
    position_units = ${position_scale}
    loss_scale = ${right_wall_ion_loss_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_target]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = target
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
    boundary = target
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
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = wafer
    electrons = em
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    r = ${wafer_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_bottom_shield]
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = bottom_shield
    electrons = em
    plasma_potential = potential
    electrode_potential = wafer_voltage_func
    r = ${bottom_shield_electron_reflection}
    position_units = ${position_scale}
    loss_ramp_time = ${electron_boundary_loss_ramp_time}
  []

  [mean_en_right_wall]
    type = HagelaarEnergyBC
    variable = mean_en
    boundary = right
    electrons = em
    r = ${right_wall_electron_reflection}
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
    type = SolutionIC
    variable = em
    solution_uo = new_bc_plasma_state
    from_variable = 'em'
    block = 0
  []

  [Cup_ic]
    type = SolutionIC
    variable = Cu+
    solution_uo = new_bc_plasma_state
    from_variable = 'Cu+'
    block = 0
  []

  [mean_en_ic]
    type = SolutionIC
    variable = mean_en
    solution_uo = new_bc_plasma_state
    from_variable = 'mean_en'
    block = 0
  []

  [potential_ic]
    type = SolutionIC
    variable = potential
    solution_uo = new_bc_plasma_state
    from_variable = 'potential'
    block = 0
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
    expression = 'tanh(t/${wafer_rf_ramp_time})*(${wafer_dc_bias} + ${wafer_rf_voltage}*sin(6.283185307179586*${wafer_rf_frequency}*t))'
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

  # In-plane R-Z B-field tables: Bx is radial Br, By is axial Bz.
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
  [new_bc_plasma_state]
    type = SolutionUserObject
    mesh = ${source_plasma_state_file}
    system_variables = 'em Cu+ mean_en potential'
    timestep = LATEST
  []

  [target_cu_ion_flux]
    type = TargetIonFluxSideUserObject
    boundary = target
    ions = Cu+
    ion_temperature = 300
    flux_model = secondary_electron_bc
    r_ion = 0
    position_units = ${position_scale}
    execute_on = 'INITIAL LINEAR TIMESTEP_END'
  []

[]

[Postprocessors]
  [target_voltage_monitor]
    type = SideAverageValue
    variable = potential
    boundary = target
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

  [wafer_sheath_voltage_average]
    type = SideAverageValue
    variable = wafer_sheath_voltage
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_sheath_thickness_average]
    type = SideAverageValue
    variable = wafer_sheath_thickness
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_flux_sheath_average]
    type = SideAverageValue
    variable = wafer_ion_flux_sheath
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_current_density_average]
    type = SideAverageValue
    variable = wafer_ion_current_density
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_ion_impact_energy_average]
    type = SideAverageValue
    variable = wafer_ion_impact_energy
    boundary = wafer
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_rf_period_fraction_average]
    type = SideAverageValue
    variable = wafer_rf_period_fraction
    boundary = wafer
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

  [zapdos_wafer_sheath_sampler]
    type = SideValueSampler
    boundary = wafer
    variable = 'wafer_sheath_voltage wafer_sheath_thickness wafer_ion_flux_sheath wafer_ion_current_density wafer_ion_impact_energy wafer_ion_transit_time wafer_rf_period_fraction'
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
  dtmax = 1.0e-8
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
