# Reduced electrostatic Ar baseline for a 60 cm PVD chamber.
# Coordinates are 2D axisymmetric RZ: x = radius [m], y = target-to-wafer axis [m].
# Target is at y = 0, wafer is at y = chamber_length.

chamber_length = 0.60
chamber_radius = 0.15

target_voltage = -730
wafer_rf_amplitude = 50
wafer_rf_frequency = 13.56e6
dc_ramp_time = 5.0e-7
rf_ramp_time = 2.0e-8
electron_temperature_bc = 3.0

gas_pressure = 0.13332235  # Pa, 1 mTorr.
argon_density = 3.22e19    # 1/m^3 at 300 K and 1 mTorr.

initial_plasma_peak = 1e15
initial_plasma_floor = 1e13
initial_mean_energy = 4.5  # eV, corresponding to initial Te = 3 eV.

position_scale = 1.0

[GlobalParams]
  potential_units = V
  use_moles = true
[]

[Mesh]
  coord_type = RZ
  rz_coord_axis = Y

  [generated]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = ${chamber_radius}
    ymin = 0
    ymax = ${chamber_length}
    nx = 24
    ny = 96
    elem_type = QUAD4
  []
[]

[Problem]
  type = FEProblem
[]

[DriftDiffusionAction]
  [Plasma]
    electrons = em
    ions = Ar+
    field = potential
    is_field_unique = true
    electron_energy = mean_en
    position_units = ${position_scale}
    additional_outputs = 'ElectronTemperature Current EField'
  []
[]

[Reactions]
  [Argon]
    species = 'em Ar+'
    aux_species = 'Ar'
    reaction_coefficient_format = 'rate'
    gas_species = 'Ar'
    electron_energy = mean_en
    electron_density = em
    include_electrons = true
    file_location = 'rate_coefficients'
    potential = potential
    use_log = true
    use_ad = true
    position_units = ${position_scale}
    block = 0
    reactions = 'em + Ar -> em + Ar*        : EEDF [-11.56] (reaction1)
                 em + Ar -> em + em + Ar+   : EEDF [-15.7] (reaction2)'
  []
[]

[AuxVariables]
  [Ar]
  []
[]

[AuxKernels]
  [Ar_background]
    type = FunctionAux
    variable = Ar
    function = argon_density_func
    execute_on = 'INITIAL TIMESTEP_BEGIN'
  []
[]

[BCs]
  [target_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = bottom
    function = target_voltage_ramp
    preset = false
  []

  [wafer_rf_potential]
    type = FunctionDirichletBC
    variable = potential
    boundary = top
    function = wafer_rf_voltage
    preset = false
  []

  [em_target]
    type = LymberopoulosElectronBC
    variable = em
    boundary = bottom
    emission_coeffs = 0.05
    ks = 1.19e5
    ions = Ar+
    position_units = ${position_scale}
  []

  [em_wafer]
    type = LymberopoulosElectronBC
    variable = em
    boundary = top
    emission_coeffs = 0.01
    ks = 1.19e5
    ions = Ar+
    position_units = ${position_scale}
  []

  [em_wall]
    type = LymberopoulosElectronBC
    variable = em
    boundary = right
    emission_coeffs = 0.01
    ks = 1.19e5
    ions = Ar+
    position_units = ${position_scale}
  []

  [Arp_target]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = bottom
    position_units = ${position_scale}
  []

  [Arp_wafer]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = top
    position_units = ${position_scale}
  []

  [Arp_wall]
    type = LymberopoulosIonBC
    variable = Ar+
    boundary = right
    position_units = ${position_scale}
  []

  [mean_en_target]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = bottom
  []

  [mean_en_wafer]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = top
  []

  [mean_en_wall]
    type = ElectronTemperatureDirichletBC
    variable = mean_en
    electrons = em
    value = ${electron_temperature_bc}
    boundary = right
  []
[]

[ICs]
  [em_ic]
    type = FunctionIC
    variable = em
    function = plasma_density_ic
  []

  [Arp_ic]
    type = FunctionIC
    variable = Ar+
    function = plasma_density_ic
  []

  [mean_en_ic]
    type = FunctionIC
    variable = mean_en
    function = mean_energy_ic
  []

  [potential_ic]
    type = FunctionIC
    variable = potential
    function = potential_ic
  []
[]

[Functions]
  [target_voltage_ramp]
    type = ParsedFunction
    expression = '${target_voltage}*tanh(t/${dc_ramp_time})'
  []

  [wafer_rf_voltage]
    type = ParsedFunction
    expression = '${wafer_rf_amplitude}*tanh(t/${rf_ramp_time})*sin(2*pi*${wafer_rf_frequency}*t)'
  []

  [argon_density_func]
    type = ParsedFunction
    expression = 'log(${argon_density}/6.022e23)'
  []

  [plasma_density_ic]
    type = ParsedFunction
    expression = 'log((${initial_plasma_floor} + ${initial_plasma_peak}*(x/${chamber_radius})^2*(1-x/${chamber_radius})^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2)/6.022e23)'
  []

  [mean_energy_ic]
    type = ParsedFunction
    expression = 'log(${initial_mean_energy}) + log((${initial_plasma_floor} + ${initial_plasma_peak}*(x/${chamber_radius})^2*(1-x/${chamber_radius})^2*(y/${chamber_length})^2*(1-y/${chamber_length})^2)/6.022e23)'
  []

  [potential_ic]
    type = ParsedFunction
    expression = '${target_voltage}*tanh(t/${dc_ramp_time}) + (${wafer_rf_amplitude}*tanh(t/${rf_ramp_time})*sin(2*pi*${wafer_rf_frequency}*t) - ${target_voltage}*tanh(t/${dc_ramp_time}))*y/${chamber_length}'
  []
[]

[Materials]
  [electron_transport]
    type = ElectronTransportCoefficients
    interp_trans_coeffs = true
    ramp_trans_coeffs = true
    pressure_dependent_electron_coeff = true
    p_gas = ${gas_pressure}
    electrons = em
    electron_energy = mean_en
    property_tables_file = rate_coefficients/electron_moments.txt
  []

  [gas_permittivity]
    type = ElectrostaticPermittivity
    potential = potential
  []

  [argon_ion]
    type = ADHeavySpecies
    heavy_species_name = Ar+
    heavy_species_mass = 6.64e-26
    heavy_species_charge = 1.0
    mobility = 143.9368369
    diffusivity = 3.98717
  []

  [argon_neutral]
    type = ADHeavySpecies
    heavy_species_name = Ar
    heavy_species_mass = 6.64e-26
    heavy_species_charge = 0.0
  []
[]

[Postprocessors]
  [target_voltage_monitor]
    type = SideAverageValue
    variable = potential
    boundary = bottom
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [wafer_voltage_monitor]
    type = SideAverageValue
    variable = potential
    boundary = top
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_temperature_average]
    type = ElementAverageValue
    variable = e_temp
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [electron_density_max]
    type = ElementExtremeValue
    variable = em_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [argon_ion_density_max]
    type = ElementExtremeValue
    variable = Ar+_density
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [argon_ion_density_min]
    type = ElementExtremeValue
    variable = Ar+_density
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [axial_efield_max]
    type = ElementExtremeValue
    variable = EFieldy
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [axial_efield_min]
    type = ElementExtremeValue
    variable = EFieldy
    value_type = min
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  automatic_scaling = true
  compute_scaling_once = false
  end_time = 3.0e-6
  dtmax = 1.0e-9
  dtmin = 1.0e-18
  scheme = bdf2
  solve_type = NEWTON
  line_search = 'bt'

  petsc_options = '-snes_converged_reason'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount'
  petsc_options_value = 'lu NONZERO 1.e-10'

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  l_max_its = 30
  nl_max_its = 60

  [TimeSteppers]
    [Adaptive]
      type = IterationAdaptiveDT
      cutback_factor = 0.4
      dt = 1.0e-13
      growth_factor = 1.1
      optimal_iterations = 20
    []
  []
[]

[Outputs]
  perf_graph = true
  [csv]
    type = CSV
    time_step_interval = 50
  []

  [exodus]
    type = Exodus
    time_step_interval = 50
  []
[]
