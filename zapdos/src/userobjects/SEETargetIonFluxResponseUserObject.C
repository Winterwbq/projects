//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SEETargetIonFluxResponseUserObject.h"

#include "Function.h"
#include "MooseUtils.h"
#include "TargetIonFluxSideUserObject.h"

#include <algorithm>
#include <cmath>
#include <limits>

registerMooseObject("ZapdosApp", SEETargetIonFluxResponseUserObject);

InputParameters
SEETargetIonFluxResponseUserObject::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>(
      "target_ion_flux", "Unnormalized Bohm target ion-flux side user object.");
  params.addRequiredParam<FunctionName>(
      "sheath_voltage", "Magnitude of the target sheath voltage in V.");
  params.addRangeCheckedParam<Real>(
      "response_time",
      "response_time > 0",
      "Physical first-order response time for ionization, ion return, and circuit feedback.");
  params.addRangeCheckedParam<Real>(
      "max_discharge_power",
      0.0,
      "max_discharge_power >= 0",
      "Maximum target discharge power in W. Zero disables the downward-only cap.");
  params.addRangeCheckedParam<Real>(
      "initial_target_ion_flux",
      0.0,
      "initial_target_ion_flux >= 0",
      "Initial filtered target ion flux. The default avoids imposing a full-strength source on "
      "the seed plasma.");
  params.addClassDescription(
      "Applies an exact first-order timestep response and a downward-only power cap to the raw "
      "target Bohm ion flux used for SEE feedback.");
  return params;
}

SEETargetIonFluxResponseUserObject::SEETargetIonFluxResponseUserObject(
    const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _target_ion_flux(getUserObject<TargetIonFluxSideUserObject>("target_ion_flux")),
    _sheath_voltage(getFunction("sheath_voltage")),
    _response_time(getParam<Real>("response_time")),
    _max_discharge_power(getParam<Real>("max_discharge_power")),
    _initial_target_ion_flux(getParam<Real>("initial_target_ion_flux")),
    _raw_target_ion_flux(0.0),
    _capped_target_ion_flux(0.0),
    _raw_discharge_power(0.0),
    _power_cap_factor(1.0),
    _filtered_target_ion_flux(declareRestartableData<Real>(
        "filtered_target_ion_flux", _initial_target_ion_flux)),
    _previous_update_time(
        declareRestartableData<Real>("previous_update_time", std::numeric_limits<Real>::max())),
    _response_initialized(declareRestartableData<bool>("response_initialized", false))
{
}

void
SEETargetIonFluxResponseUserObject::updateRawDiagnostics()
{
  _raw_target_ion_flux = std::max(_target_ion_flux.rawBohmFluxAverage(), 0.0);
  const Real voltage = std::abs(_sheath_voltage.value(_t, Point()));
  _raw_discharge_power = voltage * std::max(_target_ion_flux.rawBohmCurrent(), 0.0);
  _power_cap_factor =
      _max_discharge_power > 0.0 && _raw_discharge_power > _max_discharge_power
          ? _max_discharge_power / _raw_discharge_power
          : 1.0;
  _capped_target_ion_flux = _power_cap_factor * _raw_target_ion_flux;
}

void
SEETargetIonFluxResponseUserObject::execute()
{
  updateRawDiagnostics();

  if (!_response_initialized)
  {
    _filtered_target_ion_flux = _initial_target_ion_flux;
    _previous_update_time = _t;
    _response_initialized = true;
    return;
  }

  if (MooseUtils::absoluteFuzzyEqual(_t, _previous_update_time))
    return;

  const Real elapsed_time = _t - _previous_update_time;
  if (elapsed_time < 0.0)
    mooseError("SEE target-ion-flux response encountered decreasing simulation time.");

  const Real response_fraction = 1.0 - std::exp(-elapsed_time / _response_time);
  _filtered_target_ion_flux +=
      response_fraction * (_capped_target_ion_flux - _filtered_target_ion_flux);
  _previous_update_time = _t;
}
