# Reduced electrostatic Ar baseline with prescribed-Bz magnetized electron transport.
# Coordinates are 2D axisymmetric RZ: x = radius [m], y = target-to-wafer axis [m].
# Target is at y = 0, wafer is at y = chamber_length.

chamber_length = 0.60
chamber_radius = 0.15

target_voltage = -730
wafer_rf_amplitude = 500
wafer_rf_frequency = 13.56e6
dc_ramp_time = 5.0e-7
rf_ramp_time = 2.0e-7
electron_temperature_bc = 3.0
target_secondary_emission = 0.10
secondary_emission_ramp_time = 2.0e-7
see_source_length = 0.02
see_ionization_efficiency = 0.12
see_max_ionizations_per_secondary = 5.0
see_bulk_energy_per_pair = 5.0
magnetron_b_peak = 0.05       # Tesla, placeholder peak out-of-plane field near target.
magnetron_b_decay = 0.04      # m, axial decay length away from target.
magnetron_b_center = 0.075    # m, radial center of stronger magnetron field.
magnetron_b_width = 0.045     # m, radial width of stronger magnetron field.

gas_pressure = 0.13332235  # Pa, 1 mTorr.
argon_density = 3.22e19    # 1/m^3 at 300 K and 1 mTorr.

initial_plasma_peak = 1e15
initial_plasma_floor = 1e13
initial_mean_energy = 4.5  # eV, corresponding to initial Te = 3 eV.

position_scale = 1.0

[GlobalParams]
  potential_units = V
  use_moles = true
[]

[Mesh]
  coord_type = RZ
  rz_coord_axis = Y

  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_radius}
    ymin = 0
    ymax = ${chamber_length}
    nx = 24
    ny = 96
    elem_type = QUAD4
  []
[]

[Problem]
  type = FEProblem
[]

[DriftDiffusionAction]
  [Plasma]
    electrons = em
    ions = Ar+
    field = potential
    is_field_unique = true
    electron_energy = mean_en
    position_units = ${position_scale}
    additional_outputs = 'ElectronTemperature Current EField'
    use_magnetized_electron_transport = true
    use_magnetized_ion_transport = false
    magnetic_field_z = magnetron_bz
  []
[]

[Reactions]
  [Argon]
    species = 'em Ar+'
    aux_species = 'Ar'
    reaction_coefficient_format = 'rate'
    gas_species = 'Ar'
    electron_energy = mean_en
    electron_density = em
    include_electrons = true
    file_location = 'rate_coefficients'
    potential = potential
    use_log = true
    use_ad = true
    position_units = ${position_scale}
    block = 0
    reactions = 'em + Ar -> em + Ar*        : EEDF [-11.56] (reaction1)
                 em + Ar -> em + em + Ar+   : EEDF [-15.7] (reaction2)'
  []
[]

[Kernels]
  [see_hot_electron_source]
    type = SEESheathIonizationSource
    variable = em
    source_type = electron_density
    ions = Ar+
    sheath_voltage = target_voltage_ramp
    secondary_emission = target_secondary_gamma
    source_length = ${see_source_length}
    axial_decay_length = ${magnetron_b_decay}
    radial_center = ${magnetron_b_center}
    radial_width = ${magnetron_b_width}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    block = 0
  []

  [see_hot_argon_ion_source]
    type = SEESheathIonizationSource
    variable = Ar+
    source_type = ion_density
    ions = Ar+
    sheath_voltage = target_voltage_ramp
    secondary_emission = target_secondary_gamma
    source_length = ${see_source_length}
    axial_decay_length = ${magnetron_b_decay}
    radial_center = ${magnetron_b_center}
    radial_width = ${magnetron_b_width}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    block = 0
  []

  [see_hot_bulk_energy_source]
    type = SEESheathIonizationSource
    variable = mean_en
    source_type = electron_energy
    ions = Ar+
    sheath_voltage = target_voltage_ramp
    secondary_emission = target_secondary_gamma
    source_length = ${see_source_length}
    axial_decay_length = ${magnetron_b_decay}
    radial_center = ${magnetron_b_center}
    radial_width = ${magnetron_b_width}
    ionization_efficiency = ${see_ionization_efficiency}
    max_ionizations_per_secondary = ${see_max_ionizations_per_secondary}
    bulk_energy_per_pair = ${see_bulk_energy_per_pair}
    block = 0
  []
[]

[AuxVariables]
  [Ar]
  []

  [Bz]
  []
[]

[AuxKernels]
  [Ar_background]
    type = FunctionAux
    variable = Ar
    function = argon_density_func
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []

  [magnetic_field_z_output]
    type = FunctionAux
    variable = Bz
    function = magnetron_bz
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = bottom
    function = target_voltage_ramp
    preset = false
  []

  [wafer_rf_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = top
    function = wafer_rf_voltage
    preset = false
  []

  [em_target]
    type = HagelaarElectronBC
    variable = em
    boundary = bottom
    electron_energy = mean_en
    r = 0
    position_units = ${position_scale}
  []

  [target_secondary_electrons]
    type = SecondaryElectronBC
    variable = em
    boundary = bottom
    ions = Ar+
    electron_energy = mean_en
    r = 0
    emission_coeffs = gamma_target
    position_units = ${position_scale}
  []

  [em_wafer]
    type = LymberopoulosElectronBC
    variable = em
    boundary = top
    emission_coeffs = 0.01
    ks = 1.19e5
    ions = Ar+
    position_units = ${position_scale}
  []

  [em_wall]
    type = LymberopoulosElectronBC
    variable = em
    boundary = right
    emission_coeffs = 0.01
    ks = 1.19e5
    ions = Ar+
    position_units = ${position_scale}
  []

  [Arp_target]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = bottom
    position_units = ${position_scale}
  []

  [Arp_wafer]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = top
    position_units = ${position_scale}
  []

  [Arp_wall]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = right
    position_units = ${position_scale}
  []

  [mean_en_target]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = bottom
  []

  [mean_en_wafer]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = top
  []

  [mean_en_wall]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = right
  []
[]

[ICs]
  [em_ic]
    type = FunctionIC
    variable = em
    function = plasma_density_ic
  []

  [Arp_ic]
    type = FunctionIC
    variable = Ar+
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
  [target_voltage_ramp]
    type = ParsedFunction
    expression = '${target_voltage}*tanh(t/${dc_ramp_time})'
  []

  [wafer_rf_voltage]
    type = ParsedFunction
    expression = '${wafer_rf_amplitude}*tanh(t/${rf_ramp_time})*sin(2*pi*${wafer_rf_frequency}*t)'
  []

  [argon_density_func]
    type = ParsedFunction
    expression = 'log(${argon_density}/6.022e23)'
  []

  [magnetron_bz]
    type = ParsedFunction
    expression = '${magnetron_b_peak}*exp(-y/${magnetron_b_decay})*(0.25 + 0.75*exp(-((x-${magnetron_b_center})/${magnetron_b_width})^2))'
  []

  [target_secondary_gamma]
    type = ParsedFunction
    expression = '${target_secondary_emission}*tanh(t/${secondary_emission_ramp_time})'
  []

  [plasma_density_ic]
    type = ParsedFunction
    expression = 'log((${initial_plasma_floor} + ${initial_plasma_peak}*(x/${chamber_radius})^2*(1-x/${chamber_radius})^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2)/6.022e23)'
  []

  [mean_energy_ic]
    type = ParsedFunction
    expression = 'log(${initial_mean_energy}) + log((${initial_plasma_floor} + ${initial_plasma_peak}*(x/${chamber_radius})^2*(1-x/${chamber_radius})^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2)/6.022e23)'
  []

  [potential_ic]
    type = ParsedFunction
    expression = '${target_voltage}*tanh(t/${dc_ramp_time}) + (${wafer_rf_amplitude}*tanh(t/${rf_ramp_time})*sin(2*pi*${wafer_rf_frequency}*t) - ${target_voltage}*tanh(t/${dc_ramp_time}))*y/${chamber_length}'
  []
[]

[Materials]
  [electron_transport]
    type = ElectronTransportCoefficients
    interp_trans_coeffs = true
    ramp_trans_coeffs = true
    pressure_dependent_electron_coeff = true
    p_gas = ${gas_pressure}
    electrons = em
    electron_energy = mean_en
    property_tables_file = rate_coefficients/electron_moments.txt
  []

  [gas_permittivity]
    type = ElectrostaticPermittivity
    potential = potential
  []

  [argon_ion]
    type = ADHeavySpecies
    heavy_species_name = Ar+
    heavy_species_mass = 6.64e-26
    heavy_species_charge = 1.0
    mobility = 143.9368369
    diffusivity = 3.98717
  []

  [argon_neutral]
    type = ADHeavySpecies
    heavy_species_name = Ar
    heavy_species_mass = 6.64e-26
    heavy_species_charge = 0.0
  []

  [secondary_emission_coefficients]
    type = ADGenericFunctionMaterial
    block = 0
    prop_names = 'gamma_target'
    prop_values = 'target_secondary_gamma'
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
    boundary = top
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [target_secondary_gamma_monitor]
    type = FunctionValuePostprocessor
    function = target_secondary_gamma
    point = '${magnetron_b_center} 0 0'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_sheath_voltage_monitor]
    type = FunctionValuePostprocessor
    function = target_voltage_ramp
    point = '${magnetron_b_center} 0 0'
    scale_factor = -1
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
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_density_max]
    type = ElementExtremeValue
    variable = em_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [argon_ion_density_max]
    type = ElementExtremeValue
    variable = Ar+_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [argon_ion_density_min]
    type = ElementExtremeValue
    variable = Ar+_density
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [axial_efield_max]
    type = ElementExtremeValue
    variable = EFieldy
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [axial_efield_min]
    type = ElementExtremeValue
    variable = EFieldy
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
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
  end_time = 3.0e-6
  dtmax = 5.0e-10
  dtmin = 1.0e-18
  scheme = bdf2
  solve_type = NEWTON
  line_search = 'bt'

  petsc_options = '-snes_converged_reason'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly'

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  l_max_its = 80
  nl_max_its = 80

  [TimeSteppers]
    [Adaptive]
      type = IterationAdaptiveDT
      cutback_factor = 0.5
      dt = 1.0e-15
      growth_factor = 1.2
      optimal_iterations = 30
    []
  []
[]

[Outputs]
  perf_graph = true
  [csv]
    type = CSV
    time_step_interval = 50
  []

  [exodus]
    type = Exodus
    time_step_interval = 50
  []
[]
