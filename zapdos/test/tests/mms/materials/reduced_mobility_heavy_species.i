[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 1
  nx = 1
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [p_local]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 10
  []
  [muCu_aux]
    family = MONOMIAL
    order = CONSTANT
  []
  [diffCu_aux]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxKernels]
  [muCu_aux]
    type = ADMaterialRealAux
    variable = muCu_aux
    property = muCu+
  []
  [diffCu_aux]
    type = ADMaterialRealAux
    variable = diffCu_aux
    property = diffCu+
  []
[]

[Materials]
  [cu_ion]
    type = ADHeavySpecies
    heavy_species_name = Cu+
    heavy_species_mass = 1.0552069e-25
    heavy_species_charge = 1.0
    potential_units = V
    heavy_species_p = p_local
    reduced_mobility = 2.2e-4
    reduced_mobility_reference_pressure = 101325
    reduced_mobility_reference_temperature = 273.15
    heavy_species_T = 300
  []
[]

[Postprocessors]
  [muCu]
    type = ElementAverageValue
    variable = muCu_aux
  []
  [diffCu]
    type = ElementAverageValue
    variable = diffCu_aux
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  csv = true
  execute_on = TIMESTEP_END
[]
