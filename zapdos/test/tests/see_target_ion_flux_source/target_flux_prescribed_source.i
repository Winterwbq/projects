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
  [reference_flux_low]
    type = ParsedFunction
    expression = '2'
  []
  [reference_flux_high]
    type = ParsedFunction
    expression = '6'
  []
  [external_spatial_weight]
    type = ParsedFunction
    expression = '3'
  []
[]

[Postprocessors]
  [see_rate_low]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    prescribed_target_ion_flux = reference_flux_low
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
  []
  [see_rate_high]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    prescribed_target_ion_flux = reference_flux_high
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
  []
  [see_rate_external_weight]
    type = SEETargetIonFluxSourceIntegral
    value_type = secondary_electron_rate
    prescribed_target_ion_flux = reference_flux_low
    spatial_weight_function = external_spatial_weight
    sheath_voltage = sheath_voltage
    secondary_emission = secondary_emission
    axial_decay_length = 0.1
    target_radius = 1.0
    target_edge_width = 0.01
  []
  [prescribed_flux_rate_ratio]
    type = ParsedPostprocessor
    pp_names = 'see_rate_low see_rate_high'
    expression = 'see_rate_high / see_rate_low'
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  csv = true
[]
