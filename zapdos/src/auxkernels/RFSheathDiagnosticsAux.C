//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RFSheathDiagnosticsAux.h"

#include "Function.h"
#include "MooseVariable.h"
#include "Zapdos.h"

#include <algorithm>
#include <cmath>

registerMooseObject("ZapdosApp", RFSheathDiagnosticsAux);

InputParameters
RFSheathDiagnosticsAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  MooseEnum quantity("sheath_voltage bohm_speed ion_flux ion_current_density ion_impact_energy "
                     "sheath_thickness ion_transit_time rf_period_fraction",
                     "sheath_voltage");
  params.addRequiredCoupledVar("ions", "Ion density in log form.");
  params.addRequiredCoupledVar("electrons", "Electron density in log form.");
  params.addRequiredCoupledVar("electron_energy", "Electron energy density in log form.");
  params.addRequiredCoupledVar("plasma_potential", "Plasma potential at the sheath edge.");
  params.addRequiredParam<FunctionName>("electrode_potential",
                                        "Electrode voltage function behind the sheath.");
  params.addParam<MooseEnum>("quantity", quantity, "Sheath diagnostic quantity to compute.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addRequiredParam<bool>("use_moles", "Whether densities are stored in molar units.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier for ion collection.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the sheath-edge particle loss by tanh(t / loss_ramp_time).");
  params.addRangeCheckedParam<Real>(
      "sonic_coefficient", 1.0, "sonic_coefficient >= 0", "Multiplier on the Bohm speed.");
  params.addRangeCheckedParam<Real>(
      "ion_temperature", 300.0, "ion_temperature >= 0", "Ion temperature in K.");
  params.addRangeCheckedParam<Real>(
      "charge_state", 1.0, "charge_state > 0", "Positive ion charge state.");
  params.addRangeCheckedParam<Real>("rf_frequency", 0.0, "rf_frequency >= 0", "RF frequency.");
  params.addRangeCheckedParam<Real>("min_current_density",
                                    1.0e-30,
                                    "min_current_density > 0",
                                    "Small floor for sheath-thickness division.");
  params.addRangeCheckedParam<Real>(
      "min_velocity", 1.0e-30, "min_velocity > 0", "Small floor for transit-time division.");
  params.addClassDescription("Computes unresolved RF sheath diagnostics from the sheath-edge "
                             "plasma solution without changing the plasma equations.");
  return params;
}

RFSheathDiagnosticsAux::RFSheathDiagnosticsAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _ions(coupledValue("ions")),
    _electrons(coupledValue("electrons")),
    _electron_energy(coupledValue("electron_energy")),
    _plasma_potential(coupledValue("plasma_potential")),
    _ion_var(*getVar("ions", 0)),
    _electrode_potential(getFunction("electrode_potential")),
    _quantity(getParam<MooseEnum>("quantity")),
    _r_units(1.0 / getParam<Real>("position_units")),
    _use_moles(getParam<bool>("use_moles")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),
    _sonic_coefficient(getParam<Real>("sonic_coefficient")),
    _ion_temperature(getParam<Real>("ion_temperature")),
    _charge_state(getParam<Real>("charge_state")),
    _rf_frequency(getParam<Real>("rf_frequency")),
    _min_current_density(getParam<Real>("min_current_density")),
    _min_velocity(getParam<Real>("min_velocity")),
    _mass(getMaterialProperty<Real>("mass" + _ion_var.name()))
{
}

Real
RFSheathDiagnosticsAux::computeValue()
{
  if (_quantity == "sheath_voltage")
    return sheathVoltage();
  if (_quantity == "bohm_speed")
    return bohmSpeed();
  if (_quantity == "ion_flux")
    return particleIonFlux();
  if (_quantity == "ion_current_density")
    return ionCurrentDensity();
  if (_quantity == "ion_impact_energy")
    return ionImpactEnergy();
  if (_quantity == "sheath_thickness")
    return sheathThickness();
  if (_quantity == "ion_transit_time")
    return ionTransitTime();
  if (_quantity == "rf_period_fraction")
    return _rf_frequency > 0.0 ? ionTransitTime() * _rf_frequency : 0.0;

  mooseError("Unhandled RF sheath diagnostic quantity.");
  return 0.0;
}

Real
RFSheathDiagnosticsAux::electronTemperature() const
{
  return 2.0 / 3.0 * std::exp(_electron_energy[_qp] - _electrons[_qp]);
}

Real
RFSheathDiagnosticsAux::bohmSpeed() const
{
  const Real electron_temperature = electronTemperature();
  const Real sound_speed_squared =
      (_charge_state * ZAPDOS_CONSTANTS::e * electron_temperature +
       ZAPDOS_CONSTANTS::k_boltz * _ion_temperature) /
      _mass[_qp];

  return _sonic_coefficient * std::sqrt(std::max(sound_speed_squared, 0.0));
}

Real
RFSheathDiagnosticsAux::solverIonFlux() const
{
  return lossFactor() * _r_units * bohmSpeed() * std::exp(_ions[_qp]);
}

Real
RFSheathDiagnosticsAux::particleIonFlux() const
{
  return solverIonFlux() * (_use_moles ? ZAPDOS_CONSTANTS::N_A : 1.0);
}

Real
RFSheathDiagnosticsAux::ionCurrentDensity() const
{
  return _charge_state * ZAPDOS_CONSTANTS::e * particleIonFlux();
}

Real
RFSheathDiagnosticsAux::sheathVoltage() const
{
  return std::max(_plasma_potential[_qp] - _electrode_potential.value(_t, _q_point[_qp]), 0.0);
}

Real
RFSheathDiagnosticsAux::sheathThickness() const
{
  const Real voltage = sheathVoltage();
  if (voltage <= 0.0)
    return 0.0;

  const Real current_density = std::max(ionCurrentDensity(), _min_current_density);
  const Real child_langmuir_coefficient =
      4.0 / 9.0 * ZAPDOS_CONSTANTS::eps_0 *
      std::sqrt(2.0 * _charge_state * ZAPDOS_CONSTANTS::e / _mass[_qp]);

  return std::sqrt(child_langmuir_coefficient * std::pow(voltage, 1.5) / current_density);
}

Real
RFSheathDiagnosticsAux::ionImpactEnergy() const
{
  const Real sound_speed = bohmSpeed();
  const Real bohm_energy = 0.5 * _mass[_qp] * sound_speed * sound_speed / ZAPDOS_CONSTANTS::e;
  return bohm_energy + _charge_state * sheathVoltage();
}

Real
RFSheathDiagnosticsAux::ionTransitTime() const
{
  const Real sound_speed = bohmSpeed();
  const Real velocity_squared =
      sound_speed * sound_speed + 2.0 * _charge_state * ZAPDOS_CONSTANTS::e * sheathVoltage() /
                                      _mass[_qp];
  const Real impact_velocity = std::sqrt(std::max(velocity_squared, 0.0));
  const Real average_velocity = std::max(0.5 * (sound_speed + impact_velocity), _min_velocity);

  return sheathThickness() / average_velocity;
}

Real
RFSheathDiagnosticsAux::lossFactor() const
{
  Real factor = _loss_scale;
  if (_loss_ramp_time > 0.0)
    factor *= std::tanh(_t / _loss_ramp_time);

  return factor;
}
