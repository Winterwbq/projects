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
  [em]
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
  [reference_flux]
    type = ParsedFunction
    expression = '2'
  []
[]

[Kernels]
  [prescribed_see_source]
    type = SEETargetIonFluxSource
    variable = em
    source_type = electron_density
    prescribed_target_ion_flux = reference_flux
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
  []
[]

[Executioner]
  type = Steady
[]
