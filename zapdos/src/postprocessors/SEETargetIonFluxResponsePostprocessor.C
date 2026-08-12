//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SEETargetIonFluxResponsePostprocessor.h"

#include "SEETargetIonFluxResponseUserObject.h"

registerMooseObject("ZapdosApp", SEETargetIonFluxResponsePostprocessor);

InputParameters
SEETargetIonFluxResponsePostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  MooseEnum value_type("raw_target_ion_flux capped_target_ion_flux filtered_target_ion_flux "
                       "raw_discharge_power power_cap_factor");
  params.addRequiredParam<UserObjectName>(
      "target_ion_flux_response", "SEE target ion-flux response user object to report.");
  params.addRequiredParam<MooseEnum>(
      "value_type",
      value_type,
      "Quantity to report from the timestep-level SEE feedback response.");
  params.addClassDescription("Reports SEE target ion-flux response diagnostics.");
  return params;
}

SEETargetIonFluxResponsePostprocessor::SEETargetIonFluxResponsePostprocessor(
    const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _value_type(getParam<MooseEnum>("value_type")),
    _target_ion_flux_response(
        getUserObject<SEETargetIonFluxResponseUserObject>("target_ion_flux_response"))
{
}

Real
SEETargetIonFluxResponsePostprocessor::getValue() const
{
  if (_value_type == "raw_target_ion_flux")
    return _target_ion_flux_response.rawTargetIonFlux();
  if (_value_type == "capped_target_ion_flux")
    return _target_ion_flux_response.cappedTargetIonFlux();
  if (_value_type == "filtered_target_ion_flux")
    return _target_ion_flux_response.filteredTargetIonFlux();
  if (_value_type == "raw_discharge_power")
    return _target_ion_flux_response.rawDischargePower();
  if (_value_type == "power_cap_factor")
    return _target_ion_flux_response.powerCapFactor();

  mooseError("Unhandled SEE target ion-flux response diagnostic type.");
}
