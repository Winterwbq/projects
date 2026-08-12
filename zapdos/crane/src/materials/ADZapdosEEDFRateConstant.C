//* This file is part of Crane, an open-source
//* application for plasma chemistry and thermochemistry
//* https://github.com/lcpp-org/crane
//*
//* Crane is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADZapdosEEDFRateConstant.h"

#include "CraneUtils.h"

// MOOSE includes
#include "MooseVariable.h"

#include <cmath>

registerADMooseObject("CraneApp", ADZapdosEEDFRateConstant);

InputParameters
ADZapdosEEDFRateConstant::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params += CraneUtils::propertyFileParams();
  params.addRequiredParam<std::string>("reaction", "The full reaction equation.");
  params.addCoupledVar("mean_energy", "The electron mean energy in log form.");
  params.addCoupledVar("electrons", "The electron density.");
  params.addParam<std::string>(
      "number",
      "",
      "The reaction number. Optional, just for material property naming purposes. If a single "
      "reaction has multiple different rate coefficients (frequently the case when multiple "
      "species are lumped together to simplify a reaction network), this will prevent the same "
      "material property from being declared multiple times.");
  params.addParam<bool>(
      "clamp_actual_mean_energy",
      false,
      "Clamp exp(mean_energy - electrons) to the tabulated EEDF rate-coefficient energy range.");
  params.addParam<Real>(
      "actual_mean_energy_min",
      0.0,
      "Lower bound in eV for the actual mean electron energy used for EEDF rate interpolation. "
      "If non-positive, the first table energy is used when clamping is enabled.");
  params.addParam<Real>(
      "actual_mean_energy_max",
      0.0,
      "Upper bound in eV for the actual mean electron energy used for EEDF rate interpolation. "
      "If non-positive, the last table energy is used when clamping is enabled.");
  return params;
}

ADZapdosEEDFRateConstant::ADZapdosEEDFRateConstant(const InputParameters & parameters)
  : ADMaterial(parameters),
    _rate_coefficient(declareADProperty<Real>("k" + getParam<std::string>("number") + "_" +
                                              getParam<std::string>("reaction"))),
    _em(adCoupledValue("electrons")),
    _mean_en(isCoupled("mean_energy") ? adCoupledValue("mean_energy") : _em),
    _clamp_actual_mean_energy(getParam<bool>("clamp_actual_mean_energy")),
    _actual_mean_energy_min(getParam<Real>("actual_mean_energy_min")),
    _actual_mean_energy_max(getParam<Real>("actual_mean_energy_max"))
{
  const auto [val_x, rate_coefficient] = CraneUtils::getReactionRates(*this);
  _coefficient_interpolation.setData(val_x, rate_coefficient);

  if (_clamp_actual_mean_energy)
  {
    if (val_x.empty())
      paramError("property_file", "The EEDF rate coefficient table is empty.");
    if (_actual_mean_energy_min <= 0.0)
      _actual_mean_energy_min = val_x.front();
    if (_actual_mean_energy_max <= 0.0)
      _actual_mean_energy_max = val_x.back();
    if (_actual_mean_energy_min <= 0.0)
      paramError("actual_mean_energy_min", "The actual mean energy lower bound must be positive.");
    if (_actual_mean_energy_max <= _actual_mean_energy_min)
      paramError("actual_mean_energy_max",
                 "The actual mean energy upper bound must be greater than the lower bound.");
  }
}

ADReal
ADZapdosEEDFRateConstant::actualMeanEnergy() const
{
  ADReal actual_mean_energy;
  const Real log_actual_mean_energy = _mean_en[_qp].value() - _em[_qp].value();
  const Real value = std::exp(log_actual_mean_energy);

  if (!_clamp_actual_mean_energy)
  {
    actual_mean_energy.value() = value;
    actual_mean_energy.derivatives() =
        value * (_mean_en[_qp].derivatives() - _em[_qp].derivatives());
    return actual_mean_energy;
  }

  if (!std::isfinite(value) || value < _actual_mean_energy_min)
  {
    actual_mean_energy.value() = _actual_mean_energy_min;
    actual_mean_energy.derivatives() = 0.0;
  }
  else if (value > _actual_mean_energy_max)
  {
    actual_mean_energy.value() = _actual_mean_energy_max;
    actual_mean_energy.derivatives() = 0.0;
  }
  else
  {
    actual_mean_energy.value() = value;
    actual_mean_energy.derivatives() =
        value * (_mean_en[_qp].derivatives() - _em[_qp].derivatives());
  }

  return actual_mean_energy;
}

void
ADZapdosEEDFRateConstant::computeQpProperties()
{
  const ADReal actual_mean_energy = actualMeanEnergy();
  const Real actual_mean_energy_value = actual_mean_energy.value();

  _rate_coefficient[_qp].value() = _coefficient_interpolation.sample(actual_mean_energy_value);
  _rate_coefficient[_qp].derivatives() =
      _coefficient_interpolation.sampleDerivative(actual_mean_energy_value) *
      actual_mean_energy.derivatives();

  if (_rate_coefficient[_qp].value() < 0.0)
  {
    _rate_coefficient[_qp].value() = 0.0;
    _rate_coefficient[_qp].derivatives() = 0.0;
  }
}
