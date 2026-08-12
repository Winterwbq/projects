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
    sheath_energy_absorption_fraction = 0.25
  []
[]

[Executioner]
  type = Steady
[]
