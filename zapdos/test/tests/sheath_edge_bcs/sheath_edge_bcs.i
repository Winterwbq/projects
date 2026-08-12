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
  [em]
    initial_condition = 0
  []
  [mean_en]
    initial_condition = 0
  []
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
  [masses]
    type = GenericConstantMaterial
    prop_names = 'massem massCu+'
    prop_values = '9.1093837e-31 1.0552069e-25'
  []
[]

[Functions]
  [electrode_voltage]
    type = ParsedFunction
    expression = '-10'
  []
[]

[BCs]
  [ion_sheath_edge]
    type = SheathEdgeIonBC
    variable = Cu+
    boundary = bottom
    electrons = em
    electron_energy = mean_en
    ion_temperature = 300
    position_units = 1.0
  []

  [electron_sheath_limited]
    type = SheathLimitedElectronBC
    variable = em
    boundary = bottom
    electron_energy = mean_en
    plasma_potential = potential
    electrode_potential = electrode_voltage
    position_units = 1.0
  []

  [energy_sheath_limited]
    type = SheathLimitedEnergyBC
    variable = mean_en
    boundary = bottom
    electrons = em
    plasma_potential = potential
    electrode_potential = electrode_voltage
    position_units = 1.0
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1e-9
  dt = 1e-9
[]
