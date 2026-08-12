//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SEETargetIonFluxSource.h"

#include "Function.h"
#include "SEETargetIonFluxDepositionUserObject.h"
#include "SEETargetIonFluxResponseUserObject.h"
#include "TargetIonFluxSideUserObject.h"

#include <cmath>

registerMooseObject("ZapdosApp", SEETargetIonFluxSource);

InputParameters
SEETargetIonFluxSource::validParams()
{
  InputParameters params = ADKernel::validParams();
  MooseEnum source_type("electron_density ion_density electron_energy");
  params.addRequiredParam<MooseEnum>(
      "source_type",
      source_type,
      "Which equation receives the source: electron_density, ion_density, or electron_energy.");
  params.addParam<UserObjectName>(
      "target_ion_flux", "Optional side user object that computes incoming target ion flux.");
  params.addParam<FunctionName>(
      "prescribed_target_ion_flux",
      "Optional nonnegative target-area-average ion flux function. When supplied, the SEE source "
      "uses this reference flux instead of feedback from the computed target ion flux. Units must "
      "match the density convention (mol m^-2 s^-1 when use_moles=true).");
  params.addParam<UserObjectName>(
      "target_ion_flux_response",
      "Optional timestep-lagged, power-capped target ion flux used as the SEE feedback driver.");
  params.addParam<UserObjectName>(
      "conservative_deposition",
      "Optional user object that deposits each exact target-annulus SEE rate conservatively. "
      "When supplied, it replaces radial target-flux interpolation and spatialWeight().");
  params.addCoupledVar("potential",
                       "Optional local plasma potential in V. When supplied, the sheath voltage "
                       "is computed from potential - electrode_voltage.");
  params.addCoupledVar(
      "neutral_density",
      "Optional local neutral density used with ion_density to suppress SEE-driven ionization "
      "when the charged density exceeds the available neutral reservoir.");
  params.addCoupledVar("ion_density",
                       "Optional local ion density used with neutral_density for neutral-limited "
                       "SEE source suppression.");
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
  params.addRequiredParam<Real>("axial_decay_length",
                                "Axial decay length for the hot-electron source region.");
  MooseEnum target_location("bottom top", "bottom");
  params.addParam<MooseEnum>(
      "target_location",
      target_location,
      "Boundary location of the emitting target. Use bottom for sources decaying from the lower "
      "domain edge and top for sources decaying from the upper domain edge.");
  params.addParam<bool>(
      "use_target_flux_profile",
      false,
      "Use the local radial Bohm profile from target_ion_flux instead of a "
      "prescribed radial source window and target-wide average flux.");
  params.addRequiredParam<Real>("target_radius", "Radius of the target source footprint.");
  params.addRequiredParam<Real>("target_edge_width", "Smooth width of the target source edge.");
  MooseEnum radial_profile("disk annular_gaussian", "disk");
  params.addParam<MooseEnum>(
      "radial_profile",
      radial_profile,
      "Radial source shape: a smoothed target disk or an annular Gaussian racetrack.");
  params.addParam<Real>(
      "target_radial_center", 0.0, "Radial center of the annular Gaussian source in m.");
  params.addParam<Real>(
      "target_radial_width", 1.0, "Standard deviation of the annular Gaussian source in m.");
  params.addParam<bool>(
      "normalize_annular_profile",
      true,
      "Scale the annular Gaussian to preserve the ideal target-disk area integral in RZ.");
  params.addParam<Real>("ionization_energy", 15.76, "Ionization energy in eV.");
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
  params.addRangeCheckedParam<Real>("emission_energy",
                                    0.0,
                                    "emission_energy >= 0",
                                    "Initial energy in eV carried by each emitted secondary.");
  params.addRangeCheckedParam<Real>(
      "sheath_energy_absorption_fraction",
      0.0,
      "sheath_energy_absorption_fraction >= 0",
      "If positive for source_type=electron_energy, deposit this fraction of the secondary "
      "electron sheath energy into the bulk instead of bulk_energy_per_pair times the "
      "ionization-pair source.");
  params.addParam<bool>(
      "subtract_ionization_energy_from_absorbed_energy",
      false,
      "If true for source_type=electron_energy and positive sheath_energy_absorption_fraction, "
      "deposit only the absorbed sheath energy left after the explicit hot-beam ionization "
      "source consumes ionization_energy per represented ionization pair.");
  params.addRangeCheckedParam<Real>(
      "neutral_limiter_floor",
      1e-30,
      "neutral_limiter_floor > 0",
      "Small density floor used in neutral / (neutral + ion + floor) when neutral_density and "
      "ion_density are coupled.");
  params.addClassDescription(
      "SEE-driven volume source driven by an actual, prescribed, or lagged target ion flux.");
  return params;
}

SEETargetIonFluxSource::SEETargetIonFluxSource(const InputParameters & parameters)
  : ADKernel(parameters),
    _source_type(getParam<MooseEnum>("source_type")),
    _target_ion_flux(parameters.isParamValid("target_ion_flux")
                         ? &getUserObject<TargetIonFluxSideUserObject>("target_ion_flux")
                         : nullptr),
    _prescribed_target_ion_flux(
        parameters.isParamValid("prescribed_target_ion_flux")
            ? &getFunction("prescribed_target_ion_flux")
            : nullptr),
    _target_ion_flux_response(
        parameters.isParamValid("target_ion_flux_response")
            ? &getUserObject<SEETargetIonFluxResponseUserObject>("target_ion_flux_response")
            : nullptr),
    _conservative_deposition(
        parameters.isParamValid("conservative_deposition")
            ? &getUserObject<SEETargetIonFluxDepositionUserObject>("conservative_deposition")
            : nullptr),
    _has_potential(isCoupled("potential")),
    _potential(isCoupled("potential") ? adCoupledValue("potential") : _ad_zero),
    _use_neutral_limiter(isCoupled("neutral_density") || isCoupled("ion_density")),
    _neutral_density(isCoupled("neutral_density") ? adCoupledValue("neutral_density") : _ad_zero),
    _ion_density(isCoupled("ion_density") ? adCoupledValue("ion_density") : _ad_zero),
    _sheath_voltage(getFunction("sheath_voltage")),
    _electrode_voltage(getFunction("electrode_voltage")),
    _secondary_emission(getFunction("secondary_emission")),
    _target_location(getParam<MooseEnum>("target_location")),
    _use_target_flux_profile(getParam<bool>("use_target_flux_profile")),
    _radial_profile(getParam<MooseEnum>("radial_profile")),
    _axial_decay_length(getParam<Real>("axial_decay_length")),
    _target_radius(getParam<Real>("target_radius")),
    _target_edge_width(getParam<Real>("target_edge_width")),
    _target_radial_center(getParam<Real>("target_radial_center")),
    _target_radial_width(getParam<Real>("target_radial_width")),
    _normalize_annular_profile(getParam<bool>("normalize_annular_profile")),
    _annular_normalization(1.0),
    _ionization_energy(getParam<Real>("ionization_energy")),
    _ionization_efficiency(getParam<Real>("ionization_efficiency")),
    _max_ionizations_per_secondary(getParam<Real>("max_ionizations_per_secondary")),
    _bulk_energy_per_pair(getParam<Real>("bulk_energy_per_pair")),
    _emission_energy(getParam<Real>("emission_energy")),
    _sheath_energy_absorption_fraction(getParam<Real>("sheath_energy_absorption_fraction")),
    _subtract_ionization_energy_from_absorbed_energy(
        getParam<bool>("subtract_ionization_energy_from_absorbed_energy")),
    _neutral_limiter_floor(getParam<Real>("neutral_limiter_floor"))
{
  if (!_target_ion_flux && !_prescribed_target_ion_flux && !_target_ion_flux_response &&
      !_conservative_deposition)
    paramError("target_ion_flux",
               "Supply target_ion_flux, prescribed_target_ion_flux, target_ion_flux_response, or "
               "conservative_deposition.");
  if (_target_ion_flux_response &&
      (_target_ion_flux || _prescribed_target_ion_flux || _conservative_deposition))
    paramError("target_ion_flux_response",
               "target_ion_flux_response is an alternative to the direct, prescribed, and "
               "conservative SEE drivers.");
  if (_target_ion_flux && _prescribed_target_ion_flux)
    paramError("prescribed_target_ion_flux",
               "target_ion_flux and prescribed_target_ion_flux are alternative SEE drivers.");
  if (_prescribed_target_ion_flux && _conservative_deposition)
    paramError("prescribed_target_ion_flux",
               "Prescribed target ion flux cannot be combined with target-flux conservative "
               "deposition.");
  if (_prescribed_target_ion_flux && _use_target_flux_profile)
    paramError("use_target_flux_profile",
               "Prescribed target ion flux requires the prescribed radial_profile.");
  if (_target_ion_flux_response && _use_target_flux_profile)
    paramError("use_target_flux_profile",
               "A lagged target ion flux requires the prescribed radial_profile.");
  if (isCoupled("neutral_density") != isCoupled("ion_density"))
    paramError("neutral_density",
               "neutral_density and ion_density must be supplied together for the neutral "
               "availability limiter.");
  if (_axial_decay_length <= 0.0)
    paramError("axial_decay_length", "The axial decay length must be positive.");
  if (_target_radius <= 0.0)
    paramError("target_radius", "The target radius must be positive.");
  if (_target_edge_width <= 0.0)
    paramError("target_edge_width", "The target edge width must be positive.");
  if (_radial_profile == "annular_gaussian")
  {
    if (_target_radial_center < 0.0 || _target_radial_center > _target_radius)
      paramError("target_radial_center", "The annular center must lie within the target radius.");
    if (_target_radial_width <= 0.0)
      paramError("target_radial_width", "The annular Gaussian width must be positive.");

    if (_normalize_annular_profile)
    {
      const Real sqrt_two = std::sqrt(2.0);
      const Real u0 = -_target_radial_center / _target_radial_width;
      const Real u1 = (_target_radius - _target_radial_center) / _target_radial_width;
      const Real gaussian_area =
          _target_radial_center * _target_radial_width * std::sqrt(std::acos(-1.0) / 2.0) *
              (std::erf(u1 / sqrt_two) - std::erf(u0 / sqrt_two)) +
          _target_radial_width * _target_radial_width *
              (std::exp(-0.5 * u0 * u0) - std::exp(-0.5 * u1 * u1));
      if (gaussian_area <= 0.0)
        paramError("target_radial_width", "The annular Gaussian area must be positive.");
      _annular_normalization = 0.5 * _target_radius * _target_radius / gaussian_area;
    }
  }
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
SEETargetIonFluxSource::spatialWeight() const
{
  const Real x = _q_point[_qp](0);
  const Real y = _q_point[_qp](1);
  const Real target_distance =
      _target_location == "top" ? _mesh.getMaxInDimension(1) - y : y - _mesh.getMinInDimension(1);
  Real radial_window = 1.0;
  if (!_use_target_flux_profile && _radial_profile == "annular_gaussian")
  {
    const Real normalized_radius =
        (std::abs(x) - _target_radial_center) / _target_radial_width;
    radial_window =
        _annular_normalization * std::exp(-0.5 * normalized_radius * normalized_radius);
  }
  else if (!_use_target_flux_profile)
    radial_window =
        0.5 * (1.0 - std::tanh((std::abs(x) - _target_radius) / _target_edge_width));
  const Real domain_length =
      _mesh.getMaxInDimension(1) - _mesh.getMinInDimension(1);
  const Real axial_normalization =
      _use_target_flux_profile
          ? _axial_decay_length * (1.0 - std::exp(-domain_length / _axial_decay_length))
          : _axial_decay_length;
  return radial_window * std::exp(-target_distance / _axial_decay_length) /
         axial_normalization;
}

ADReal
SEETargetIonFluxSource::positivePart(const ADReal & value) const
{
  return value.value() > 0.0 ? value : 0.0;
}

ADReal
SEETargetIonFluxSource::neutralLimiter() const
{
  if (!_use_neutral_limiter)
    return 1.0;

  const ADReal neutral = positivePart(_neutral_density[_qp] - _neutral_limiter_floor) +
                         _neutral_limiter_floor;
  const ADReal ion = positivePart(_ion_density[_qp]);
  return neutral / (neutral + ion + _neutral_limiter_floor);
}

ADReal
SEETargetIonFluxSource::sheathVoltage() const
{
  if (_has_potential)
  {
    const Real electrode_voltage = _electrode_voltage.value(_t, _q_point[_qp]);
    return positivePart(_potential[_qp] - electrode_voltage);
  }

  return std::abs(_sheath_voltage.value(_t, _q_point[_qp]));
}

ADReal
SEETargetIonFluxSource::ionizationYield() const
{
  const ADReal available_energy = positivePart(sheathVoltage() - _ionization_energy);
  const ADReal yield = _ionization_efficiency * available_energy / _ionization_energy;
  return yield.value() < _max_ionizations_per_secondary ? yield : _max_ionizations_per_secondary;
}

ADReal
SEETargetIonFluxSource::secondaryElectronVolumeRate() const
{
  if (_conservative_deposition)
    return _conservative_deposition->secondaryElectronVolumeRate(_q_point[_qp]);

  const Real gamma = std::max(_secondary_emission.value(_t, _q_point[_qp]), 0.0);
  const Real target_flux =
      _target_ion_flux_response
          ? _target_ion_flux_response->filteredTargetIonFlux()
          : (_prescribed_target_ion_flux
                 ? std::max(_prescribed_target_ion_flux->value(_t, _q_point[_qp]), 0.0)
                 : (_use_target_flux_profile
                        ? _target_ion_flux->normalizedFluxAtRadius(_q_point[_qp](0))
                        : _target_ion_flux->incomingFluxAverage()));
  return gamma * target_flux * spatialWeight();
}

ADReal
SEETargetIonFluxSource::computeQpResidual()
{
  const ADReal limiter = neutralLimiter();
  ADReal source = secondaryElectronVolumeRate() * ionizationYield() * limiter;
  if (_source_type == "electron_energy")
  {
    if (_sheath_energy_absorption_fraction > 0.0 || _emission_energy > 0.0)
    {
      const ADReal absorbed_energy =
          _emission_energy + _sheath_energy_absorption_fraction * sheathVoltage();
      const ADReal thermalized_energy =
          _subtract_ionization_energy_from_absorbed_energy
              ? positivePart(absorbed_energy - ionizationYield() * _ionization_energy)
              : absorbed_energy;
      source = secondaryElectronVolumeRate() * thermalized_energy * limiter;
    }
    else
      source *= _bulk_energy_per_pair;
  }

  return -_test[_i][_qp] * source;
}
