//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SheathLimitedElectronBC.h"
#include "Function.h"
#include "Zapdos.h"

registerADMooseObject("ZapdosApp", SheathLimitedElectronBC);

InputParameters
SheathLimitedElectronBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredCoupledVar("electron_energy", "The electron energy density in log form.");
  params.addRequiredCoupledVar("plasma_potential", "The sheath-edge plasma potential.");
  params.addRequiredParam<FunctionName>("electrode_potential", "The metal electrode voltage.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addRangeCheckedParam<Real>("r", 0.0, "r >= 0 & r < 1", "Thermal reflection coefficient.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier for electron loss.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).");
  params.addClassDescription("Sheath-limited electron thermal loss using electrode voltage as a "
                             "barrier, without local E-field collection.");
  return params;
}

SheathLimitedElectronBC::SheathLimitedElectronBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),
    _electron_energy(adCoupledValue("electron_energy")),
    _plasma_potential(adCoupledValue("plasma_potential")),
    _electrode_potential(getFunction("electrode_potential")),
    _mass(getMaterialProperty<Real>("mass" + _var.name()))
{
}

ADReal
SheathLimitedElectronBC::sheathTransmission() const
{
  using std::exp;

  ADReal transmission = 1.0;
  const ADReal mean_energy = exp(_electron_energy[_qp] - _u[_qp]);
  const ADReal electron_temperature = 2.0 / 3.0 * mean_energy;
  const ADReal barrier = _plasma_potential[_qp] - _electrode_potential.value(_t, _q_point[_qp]);

  if (barrier > 0.0)
    transmission = exp(-barrier / electron_temperature);

  return transmission;
}

ADReal
SheathLimitedElectronBC::computeQpResidual()
{
  using std::exp;
  using std::sqrt;

  const ADReal mean_energy = exp(_electron_energy[_qp] - _u[_qp]);
  const ADReal electron_temperature = 2.0 / 3.0 * mean_energy;
  const ADReal v_thermal =
      sqrt(8 * ZAPDOS_CONSTANTS::e * electron_temperature / (libMesh::pi * _mass[_qp]));

  Real loss_factor = _loss_scale * (1. - _r) / (1. + _r);
  if (_loss_ramp_time > 0.0)
    loss_factor *= std::tanh(_t / _loss_ramp_time);

  return loss_factor * _test[_i][_qp] * _r_units * 0.25 * v_thermal *
         sheathTransmission() * exp(_u[_qp]);
}
