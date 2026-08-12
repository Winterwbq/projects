[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  nx = 3
  ny = 2
  coord_type = RZ
  rz_coord_axis = Y
[]

[Variables]
  [Cu+]
  []
  [em]
    initial_condition = 0
  []
  [mean_en]
    initial_condition = 1.791759469228055
  []
  [potential]
    initial_condition = 0
  []
[]

[ICs]
  [cu_ion_profile]
    type = FunctionIC
    variable = Cu+
    function = cu_ion_profile
  []
[]

[Problem]
  kernel_coverage_check = false
[]

[Materials]
  [field_solver]
    type = FieldSolverMaterial
    potential = potential
  []

  [cu_ion]
    type = ADHeavySpecies
    heavy_species_name = Cu+
    heavy_species_mass = 1.0552069e-25
    heavy_species_charge = 1.0
    potential_units = V
    mobility = 1.0
    diffusivity = 1.0
  []
[]

[Functions]
  [commanded_current]
    type = ParsedFunction
    expression = '1'
  []
  [sheath_voltage]
    type = ParsedFunction
    expression = '100'
  []
  [secondary_emission]
    type = ParsedFunction
    expression = '0.05 + 0.15*x'
  []
  [cu_ion_profile]
    type = ParsedFunction
    expression = 'log(1 + 4*x*x)'
  []
[]

[UserObjects]
  [target_ion_flux]
    type = TargetIonFluxSideUserObject
    boundary = top
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    flux_model = current_normalized_bohm
    use_moles = true
    secondary_emission = secondary_emission
    commanded_current = commanded_current
    current_floor = 1e-12
    max_normalization = 1e12
    position_units = 1
    execution_order_group = -1
    execute_on = 'INITIAL LINEAR NONLINEAR TIMESTEP_END'
  []

  [conservative_deposition]
    type = SEETargetIonFluxDepositionUserObject
    target_ion_flux = target_ion_flux
    target_location = top
    axial_decay_length = 0.05
    position_units = 1
    execution_order_group = 0
    execute_on = 'INITIAL LINEAR NONLINEAR TIMESTEP_END'
  []
[]

[Postprocessors]
  [target_see_rate]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_ion_flux
    value_type = secondary_electron_rate
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [volume_see_rate]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    target_ion_flux = target_ion_flux
    conservative_deposition = conservative_deposition
    use_target_flux_profile = true
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    target_location = top
    axial_decay_length = 0.05
    target_radius = 1
    target_edge_width = 0.01
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_rate_relative_difference]
    type = RelativeDifferencePostprocessor
    value1 = volume_see_rate
    value2 = target_see_rate
    execution_order_group = 2
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  csv = true
  show = 'see_rate_relative_difference'
[]
