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

[UserObjects]
  [target_ion_flux]
    type = TargetIonFluxSideUserObject
    boundary = bottom
    ions = Cu+
    ion_temperature = 300
    flux_model = secondary_electron_bc
    position_units = 1.0
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Postprocessors]
  [target_flux_average]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_ion_flux
    value_type = average
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [target_flux_integral]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_ion_flux
    value_type = integral
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1e-5
  dt = 1e-5
[]

[Outputs]
  csv = true
[]
