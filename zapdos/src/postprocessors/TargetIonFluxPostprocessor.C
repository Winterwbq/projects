//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TargetIonFluxPostprocessor.h"

#include "TargetIonFluxSideUserObject.h"

registerMooseObject("ZapdosApp", TargetIonFluxPostprocessor);

InputParameters
TargetIonFluxPostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  MooseEnum value_type("average integral area raw_bohm_current normalization_factor ion_current "
                       "discharge_current commanded_current secondary_electron_rate");
  params.addRequiredParam<UserObjectName>(
      "target_ion_flux", "Target ion flux side user object to report.");
  params.addRequiredParam<MooseEnum>(
      "value_type",
      value_type,
      "Quantity to report from the target ion-flux and current-normalization model.");
  params.addClassDescription("Reports target ion flux diagnostics.");
  return params;
}

TargetIonFluxPostprocessor::TargetIonFluxPostprocessor(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _value_type(getParam<MooseEnum>("value_type")),
    _target_ion_flux(getUserObject<TargetIonFluxSideUserObject>("target_ion_flux"))
{
}

Real
TargetIonFluxPostprocessor::getValue() const
{
  if (_value_type == "average")
    return _target_ion_flux.incomingFluxAverage();
  if (_value_type == "integral")
    return _target_ion_flux.incomingFluxIntegral();
  if (_value_type == "area")
    return _target_ion_flux.targetArea();
  if (_value_type == "raw_bohm_current")
    return _target_ion_flux.rawBohmCurrent();
  if (_value_type == "normalization_factor")
    return _target_ion_flux.normalizationFactor();
  if (_value_type == "ion_current")
    return _target_ion_flux.ionCurrent();
  if (_value_type == "discharge_current")
    return _target_ion_flux.dischargeCurrent();
  if (_value_type == "commanded_current")
    return _target_ion_flux.commandedCurrent();
  if (_value_type == "secondary_electron_rate")
    return _target_ion_flux.secondaryElectronRate();

  mooseError("Unhandled target ion flux diagnostic type.");
}
