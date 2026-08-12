//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SEESheathIonizationSource.h"
#include "Function.h"
#include "Zapdos.h"

registerMooseObject("ZapdosApp", SEESheathIonizationSource);

InputParameters
SEESheathIonizationSource::validParams()
{
  InputParameters params = ADKernel::validParams();
  MooseEnum source_type("electron_density ion_density electron_energy");
  params.addRequiredParam<MooseEnum>(
      "source_type",
      source_type,
      "Which equation receives the source: electron_density, ion_density, or electron_energy.");
  params.addRequiredCoupledVar("ions", "Ion density in log form that drives target bombardment.");
  params.addCoupledVar("potential",
                       "Optional local plasma potential in V. When supplied, the sheath voltage "
                       "is computed from potential - electrode_voltage instead of the prescribed "
                       "sheath_voltage function.");
  params.addRequiredParam<FunctionName>(
      "sheath_voltage",
      "Fallback function giving the cathode sheath voltage scale in V. The absolute value is used "
      "when potential is not coupled.");
  params.addParam<FunctionName>(
      "electrode_voltage",
      "0",
      "Electrode voltage in V used with the coupled potential to compute the local sheath drop.");
  params.addRequiredParam<FunctionName>(
      "secondary_emission",
      "Function giving the effective secondary-electron emission coefficient.");
  params.addRequiredParam<Real>("source_length",
                                "Length scale used to convert ion wall flux to a volumetric "
                                "near-target source.");
  params.addRequiredParam<Real>("axial_decay_length",
                                "Axial decay length for the hot-electron source region.");
  params.addRequiredParam<Real>("radial_center", "Radial center of the magnetron source region.");
  params.addRequiredParam<Real>("radial_width", "Radial Gaussian width of the source region.");
  params.addParam<Real>("ion_mass", 6.64e-26, "Ion mass in kg.");
  params.addParam<Real>(
      "bohm_electron_temperature",
      3.0,
      "Electron temperature in eV used for the Bohm-like ion flux estimate.");
  params.addParam<Real>("ionization_energy", 15.76, "Argon ionization energy in eV.");
  params.addParam<Real>(
      "ionization_efficiency",
      0.15,
      "Fraction of secondary-electron sheath energy converted into ionizing collisions.");
  params.addParam<Real>("max_ionizations_per_secondary",
                        5.0,
                        "Upper bound on ionization events represented per emitted secondary.");
  params.addParam<Real>(
      "bulk_energy_per_pair",
      3.0,
      "Energy in eV deposited into the bulk electron energy equation per created pair.");
  params.addRangeCheckedParam<Real>(
      "sheath_energy_absorption_fraction",
      0.0,
      "sheath_energy_absorption_fraction >= 0",
      "If positive for source_type=electron_energy, deposit this fraction of the secondary "
      "electron sheath energy into the bulk instead of bulk_energy_per_pair times the "
      "ionization-pair source.");
  params.addClassDescription(
      "Approximate SEE-driven hot-electron ionization source for magnetron startup models.");
  return params;
}

SEESheathIonizationSource::SEESheathIonizationSource(const InputParameters & parameters)
  : ADKernel(parameters),
    _source_type(getParam<MooseEnum>("source_type")),
    _ions(adCoupledValue("ions")),
    _has_potential(isCoupled("potential")),
    _potential(isCoupled("potential") ? adCoupledValue("potential") : _ad_zero),
    _sheath_voltage(getFunction("sheath_voltage")),
    _electrode_voltage(getFunction("electrode_voltage")),
    _secondary_emission(getFunction("secondary_emission")),
    _ion_mass(getParam<Real>("ion_mass")),
    _bohm_electron_temperature(getParam<Real>("bohm_electron_temperature")),
    _source_length(getParam<Real>("source_length")),
    _axial_decay_length(getParam<Real>("axial_decay_length")),
    _radial_center(getParam<Real>("radial_center")),
    _radial_width(getParam<Real>("radial_width")),
    _ionization_energy(getParam<Real>("ionization_energy")),
    _ionization_efficiency(getParam<Real>("ionization_efficiency")),
    _max_ionizations_per_secondary(getParam<Real>("max_ionizations_per_secondary")),
    _bulk_energy_per_pair(getParam<Real>("bulk_energy_per_pair")),
    _sheath_energy_absorption_fraction(getParam<Real>("sheath_energy_absorption_fraction"))
{
  if (_ion_mass <= 0.0)
    paramError("ion_mass", "The ion mass must be positive.");
  if (_bohm_electron_temperature <= 0.0)
    paramError("bohm_electron_temperature", "The Bohm electron temperature must be positive.");
  if (_source_length <= 0.0)
    paramError("source_length", "The source length must be positive.");
  if (_axial_decay_length <= 0.0)
    paramError("axial_decay_length", "The axial decay length must be positive.");
  if (_radial_width <= 0.0)
    paramError("radial_width", "The radial width must be positive.");
  if (_ionization_energy <= 0.0)
    paramError("ionization_energy", "The ionization energy must be positive.");
  if (_ionization_efficiency < 0.0)
    paramError("ionization_efficiency", "The ionization efficiency cannot be negative.");
  if (_max_ionizations_per_secondary < 0.0)
    paramError("max_ionizations_per_secondary",
               "The maximum ionizations per secondary cannot be negative.");
  if (_bulk_energy_per_pair < 0.0)
    paramError("bulk_energy_per_pair", "The bulk energy per pair cannot be negative.");
}

Real
SEESheathIonizationSource::spatialWeight() const
{
  const Real x = _q_point[_qp](0);
  const Real y = _q_point[_qp](1);
  const Real radial_arg = (x - _radial_center) / _radial_width;
  return std::exp(-y / _axial_decay_length) * std::exp(-radial_arg * radial_arg);
}

ADReal
SEESheathIonizationSource::positivePart(const ADReal & value) const
{
  return value.value() > 0.0 ? value : 0.0;
}

ADReal
SEESheathIonizationSource::sheathVoltage() const
{
  if (_has_potential)
  {
    const Real electrode_voltage = _electrode_voltage.value(_t, _q_point[_qp]);
    return positivePart(_potential[_qp] - electrode_voltage);
  }

  return std::abs(_sheath_voltage.value(_t, _q_point[_qp]));
}

ADReal
SEESheathIonizationSource::ionizationYield() const
{
  const ADReal available_energy = positivePart(sheathVoltage() - _ionization_energy);
  const ADReal yield = _ionization_efficiency * available_energy / _ionization_energy;
  return yield.value() < _max_ionizations_per_secondary ? yield : _max_ionizations_per_secondary;
}

ADReal
SEESheathIonizationSource::secondaryElectronRate() const
{
  using std::exp;
  using std::sqrt;

  const Real gamma = std::max(_secondary_emission.value(_t, _q_point[_qp]), 0.0);
  const Real bohm_velocity =
      sqrt(ZAPDOS_CONSTANTS::e * _bohm_electron_temperature / _ion_mass);
  const Real coeff = gamma * bohm_velocity / _source_length * spatialWeight();

  return coeff * exp(_ions[_qp]);
}

ADReal
SEESheathIonizationSource::sourceRate() const
{
  return secondaryElectronRate() * ionizationYield();
}

ADReal
SEESheathIonizationSource::computeQpResidual()
{
  ADReal source = sourceRate();
  if (_source_type == "electron_energy")
  {
    if (_sheath_energy_absorption_fraction > 0.0)
      source = secondaryElectronRate() * _sheath_energy_absorption_fraction * sheathVoltage();
    else
      source *= _bulk_energy_per_pair;
  }

  return -_test[_i][_qp] * source;
}
