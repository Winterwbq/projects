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
    initial_condition = 0
  []
  [mean_en]
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

[Kernels]
  [see_energy]
    type = SEETargetIonFluxSource
    variable = mean_en
    source_type = electron_energy
    target_ion_flux = target_ion_flux
    potential = potential
    sheath_voltage = sheath_voltage
    electrode_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
    ionization_energy = 7.73
    ionization_efficiency = 0.3
    max_ionizations_per_secondary = 20
    sheath_energy_absorption_fraction = 1.0
  []
[]

[Postprocessors]
  [target_flux_average]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_ion_flux
    value_type = average
  []
  [target_flux_integral]
    type = TargetIonFluxPostprocessor
    target_ion_flux = target_ion_flux
    value_type = integral
  []
  [see_secondary_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    target_ion_flux = target_ion_flux
    potential = potential
    sheath_voltage = sheath_voltage
    electrode_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
    ionization_energy = 7.73
    ionization_efficiency = 0.3
    max_ionizations_per_secondary = 20
    sheath_energy_absorption_fraction = 1.0
  []
  [see_ionization_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = ionization_rate
    target_ion_flux = target_ion_flux
    potential = potential
    sheath_voltage = sheath_voltage
    electrode_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
    ionization_energy = 7.73
    ionization_efficiency = 0.3
    max_ionizations_per_secondary = 20
    sheath_energy_absorption_fraction = 1.0
  []
  [see_energy_rate_integral]
    type = SEETargetIonFluxSourceIntegral
    value_type = electron_energy_rate
    target_ion_flux = target_ion_flux
    potential = potential
    sheath_voltage = sheath_voltage
    electrode_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
    ionization_energy = 7.73
    ionization_efficiency = 0.3
    max_ionizations_per_secondary = 20
    sheath_energy_absorption_fraction = 1.0
  []
[]

[Executioner]
  type = Steady
[]
