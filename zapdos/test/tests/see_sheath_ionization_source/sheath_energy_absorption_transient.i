[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
  xmin = 0
  xmax = 1
[]

[Variables]
  [mean_en]
    initial_condition = 0
  []
  [Cu+]
    initial_condition = 0
  []
[]

[Problem]
  kernel_coverage_check = false
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

[Kernels]
  [mean_en_time]
    type = TimeDerivativeLog
    variable = mean_en
  []

  [cu_ion_time]
    type = TimeDerivativeLog
    variable = Cu+
  []

  [see_energy]
    type = SEESheathIonizationSource
    variable = mean_en
    source_type = electron_energy
    ions = Cu+
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    source_length = 1
    axial_decay_length = 1
    radial_center = 0
    radial_width = 1
    ion_mass = 1.0552069e-25
    ionization_energy = 7.73
    ionization_efficiency = 0.3
    max_ionizations_per_secondary = 20
    bulk_energy_per_pair = 1
    sheath_energy_absorption_fraction = 1
  []
[]

[Postprocessors]
  [mean_en_average]
    type = ElementAverageValue
    variable = mean_en
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
