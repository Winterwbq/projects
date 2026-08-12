//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SheathEdgeIonBC.h"
#include "TargetIonFluxSideUserObject.h"
#include "Zapdos.h"

registerADMooseObject("ZapdosApp", SheathEdgeIonBC);

InputParameters
SheathEdgeIonBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredCoupledVar("electrons", "The electron density in log form.");
  params.addRequiredCoupledVar("electron_energy", "The electron energy density in log form.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier for ion collection.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).");
  params.addRangeCheckedParam<Real>(
      "sonic_coefficient", 1.0, "sonic_coefficient >= 0", "Multiplier on the Bohm speed.");
  params.addRangeCheckedParam<Real>(
      "ion_temperature", 300.0, "ion_temperature >= 0", "Ion temperature in K.");
  params.addParam<UserObjectName>(
      "target_ion_flux",
      "Optional target ion-flux user object supplying the shared current-normalization factor.");
  params.addClassDescription("Bohm-like ion loss at a sheath edge without local E-field "
                             "collection from the unresolved sheath.");
  return params;
}

SheathEdgeIonBC::SheathEdgeIonBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),
    _sonic_coefficient(getParam<Real>("sonic_coefficient")),
    _ion_temperature(getParam<Real>("ion_temperature")),
    _target_ion_flux(isParamValid("target_ion_flux")
                         ? &getUserObject<TargetIonFluxSideUserObject>("target_ion_flux")
                         : nullptr),
    _electrons(adCoupledValue("electrons")),
    _electron_energy(adCoupledValue("electron_energy")),
    _mass(getMaterialProperty<Real>("mass" + _var.name()))
{
}

ADReal
SheathEdgeIonBC::computeQpResidual()
{
  using std::exp;
  using std::sqrt;

  const ADReal mean_energy = exp(_electron_energy[_qp] - _electrons[_qp]);
  const ADReal electron_temperature = 2.0 / 3.0 * mean_energy;
  const ADReal sound_speed =
      _sonic_coefficient *
      sqrt((ZAPDOS_CONSTANTS::e * electron_temperature +
            ZAPDOS_CONSTANTS::k_boltz * _ion_temperature) /
           _mass[_qp]);

  Real loss_factor = _loss_scale;
  if (_loss_ramp_time > 0.0)
    loss_factor *= std::tanh(_t / _loss_ramp_time);
  if (_target_ion_flux)
    loss_factor *= _target_ion_flux->normalizationFactor();

  return loss_factor * _test[_i][_qp] * _r_units * sound_speed * exp(_u[_qp]);
}
