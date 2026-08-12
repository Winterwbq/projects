[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  nx = 1
  ny = 1
[]

[Variables]
  [Cu+]
    initial_condition = 0
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
  [sheath_voltage]
    type = ParsedFunction
    expression = '100'
  []
  [secondary_emission]
    type = ParsedFunction
    expression = '0.1'
  []
[]

[UserObjects]
  [target_ion_flux]
    type = TargetIonFluxSideUserObject
    boundary = bottom
    ions = Cu+
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    flux_model = bohm
    secondary_emission = secondary_emission
    position_units = 1
    execution_order_group = -1
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [see_flux_response]
    type = SEETargetIonFluxResponseUserObject
    target_ion_flux = target_ion_flux
    sheath_voltage = sheath_voltage
    response_time = 1
    max_discharge_power = 1e-14
    execution_order_group = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Postprocessors]
  [raw_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_flux_response
    value_type = raw_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [capped_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_flux_response
    value_type = capped_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [filtered_flux]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_flux_response
    value_type = filtered_target_ion_flux
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [raw_power]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_flux_response
    value_type = raw_discharge_power
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [cap_factor]
    type = SEETargetIonFluxResponsePostprocessor
    target_ion_flux_response = see_flux_response
    value_type = power_cap_factor
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [response_fraction]
    type = ParsedPostprocessor
    pp_names = 'filtered_flux capped_flux'
    expression = 'filtered_flux / capped_flux'
    execution_order_group = 2
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [cap_identity]
    type = ParsedPostprocessor
    pp_names = 'raw_flux capped_flux raw_power'
    expression = 'capped_flux * raw_power / (raw_flux * 1e-14)'
    execution_order_group = 2
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [volume_see_rate]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    target_ion_flux_response = see_flux_response
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1
    target_edge_width = 0.01
    execution_order_group = 1
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
  dt = 0.5
[]

[Outputs]
  csv = true
  show = 'cap_factor response_fraction cap_identity volume_see_rate'
[]
