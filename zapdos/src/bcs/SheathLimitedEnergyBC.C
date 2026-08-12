//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SheathLimitedEnergyBC.h"
#include "Function.h"
#include "Zapdos.h"

registerADMooseObject("ZapdosApp", SheathLimitedEnergyBC);

InputParameters
SheathLimitedEnergyBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredCoupledVar("electrons", "The electron density in log form.");
  params.addRequiredCoupledVar("plasma_potential", "The sheath-edge plasma potential.");
  params.addRequiredParam<FunctionName>("electrode_potential", "The metal electrode voltage.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addRangeCheckedParam<Real>("r", 0.0, "r >= 0 & r < 1", "Thermal reflection coefficient.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier for energy loss.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the boundary energy loss by tanh(t / loss_ramp_time).");
  params.addRangeCheckedParam<Real>("energy_loss_coefficient",
                                    5.0 / 3.0,
                                    "energy_loss_coefficient >= 0",
                                    "Multiplier for convected electron energy loss.");
  params.addClassDescription("Sheath-limited electron energy loss using the same barrier model as "
                             "SheathLimitedElectronBC.");
  return params;
}

SheathLimitedEnergyBC::SheathLimitedEnergyBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),
    _energy_loss_coefficient(getParam<Real>("energy_loss_coefficient")),
    _electrons(adCoupledValue("electrons")),
    _plasma_potential(adCoupledValue("plasma_potential")),
    _electrode_potential(getFunction("electrode_potential")),
    _mass(getMaterialProperty<Real>("mass" + (*getVar("electrons", 0)).name()))
{
}

ADReal
SheathLimitedEnergyBC::sheathTransmission() const
{
  using std::exp;

  ADReal transmission = 1.0;
  const ADReal mean_energy = exp(_u[_qp] - _electrons[_qp]);
  const ADReal electron_temperature = 2.0 / 3.0 * mean_energy;
  const ADReal barrier = _plasma_potential[_qp] - _electrode_potential.value(_t, _q_point[_qp]);

  if (barrier > 0.0)
    transmission = exp(-barrier / electron_temperature);

  return transmission;
}

ADReal
SheathLimitedEnergyBC::computeQpResidual()
{
  using std::exp;
  using std::sqrt;

  const ADReal mean_energy = exp(_u[_qp] - _electrons[_qp]);
  const ADReal electron_temperature = 2.0 / 3.0 * mean_energy;
  const ADReal v_thermal =
      sqrt(8 * ZAPDOS_CONSTANTS::e * electron_temperature / (libMesh::pi * _mass[_qp]));

  Real loss_factor = _loss_scale * (1. - _r) / (1. + _r);
  if (_loss_ramp_time > 0.0)
    loss_factor *= std::tanh(_t / _loss_ramp_time);

  return loss_factor * _test[_i][_qp] * _r_units * _energy_loss_coefficient * 0.25 *
         v_thermal * sheathTransmission() * exp(_u[_qp]);
}
