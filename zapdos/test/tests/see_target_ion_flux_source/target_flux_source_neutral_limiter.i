[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  nx = 2
  ny = 2
[]

[Variables]
  [Cu+]
    initial_condition = 1
  []
  [potential]
    initial_condition = -100
  []
[]

[AuxVariables]
  [neutral_density]
    initial_condition = 9
  []
  [ion_density]
    initial_condition = 1
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
    position_units = 1.0
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Postprocessors]
  [see_neutral_limiter_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = neutral_limiter
    target_ion_flux = target_ion_flux
    potential = potential
    sheath_voltage = sheath_voltage
    electrode_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 1
    target_radius = 1
    target_edge_width = 0.01
    neutral_density = neutral_density
    ion_density = ion_density
    neutral_limiter_floor = 1e-30
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  csv = true
[]
