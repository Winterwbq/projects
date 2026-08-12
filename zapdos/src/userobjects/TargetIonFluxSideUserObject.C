//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TargetIonFluxSideUserObject.h"

#include "Function.h"
#include "MooseVariable.h"
#include "Zapdos.h"

#include <algorithm>
#include <cmath>
#include <numeric>

using MetaPhysicL::raw_value;

registerMooseObject("ZapdosApp", TargetIonFluxSideUserObject);

InputParameters
TargetIonFluxSideUserObject::validParams()
{
  InputParameters params = SideUserObject::validParams();
  MooseEnum flux_model(
      "drift_only secondary_electron_bc bohm current_normalized_bohm", "drift_only");
  params.addRequiredCoupledVar("ions", "Ion density in log form.");
  params.addCoupledVar("ion_temperature", 300, "Ion temperature in Kelvin.");
  params.addCoupledVar(
      "electrons", "Bulk electron density in log form for the Bohm sound speed.");
  params.addCoupledVar(
      "electron_energy", "Bulk electron energy density in log form for the Bohm sound speed.");
  params.addParam<MooseEnum>(
      "flux_model",
      flux_model,
      "Flux model used for the incoming target ion flux. 'drift_only' preserves the original "
      "drift-to-wall diagnostic, while 'secondary_electron_bc' uses the same ion-impact flux "
      "expression as SecondaryElectronBC. 'bohm' evaluates the unnormalized local Bohm flux. "
      "'current_normalized_bohm' evaluates that Bohm profile and normalizes it to a commanded "
      "total discharge current.");
  params.addParam<Real>("r_ion", 0, "Ion reflection coefficient for secondary_electron_bc flux.");
  params.addParam<bool>("use_moles", false, "Whether the density variables use molar units.");
  params.addParam<FunctionName>(
      "secondary_emission", "0", "Secondary-electron emission coefficient gamma.");
  params.addParam<FunctionName>(
      "commanded_current", "0", "Commanded total ion-plus-SEE discharge current in A.");
  params.addRangeCheckedParam<Real>(
      "current_floor", 1e-12, "current_floor > 0", "Minimum raw current denominator in A.");
  params.addRangeCheckedParam<Real>("max_normalization",
                                    1e12,
                                    "max_normalization > 0",
                                    "Maximum current-normalization factor during startup.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addClassDescription("Computes the incoming positive-ion flux to a target boundary.");
  return params;
}

TargetIonFluxSideUserObject::TargetIonFluxSideUserObject(const InputParameters & parameters)
  : SideUserObject(parameters),
    _ion_var(*getVar("ions", 0)),
    _ions(coupledValue("ions")),
    _ion_temperature(coupledValue("ion_temperature")),
    _electrons(isCoupled("electrons") ? &coupledValue("electrons") : nullptr),
    _electron_energy(isCoupled("electron_energy") ? &coupledValue("electron_energy") : nullptr),
    _flux_model(getParam<MooseEnum>("flux_model")),
    _position_units(getParam<Real>("position_units")),
    _r_units(1.0 / _position_units),
    _r_ion(getParam<Real>("r_ion")),
    _use_moles(getParam<bool>("use_moles")),
    _secondary_emission(getFunction("secondary_emission")),
    _commanded_current(getFunction("commanded_current")),
    _current_floor(getParam<Real>("current_floor")),
    _max_normalization(getParam<Real>("max_normalization")),
    _mu(getADMaterialProperty<Real>("mu" + _ion_var.name())),
    _sgn(getMaterialProperty<Real>("sgn" + _ion_var.name())),
    _mass(getMaterialProperty<Real>("mass" + _ion_var.name())),
    _electric_field(
        getADMaterialProperty<RealVectorValue>(getParam<std::string>("field_property_name"))),
    _incoming_flux_integral(0.0),
    _target_area(0.0),
    _raw_bohm_flux_integral(0.0),
    _raw_bohm_current(0.0),
    _raw_secondary_electron_rate(0.0),
    _normalization_factor(1.0),
    _ion_current(0.0),
    _discharge_current(0.0),
    _commanded_current_value(0.0),
    _secondary_electron_rate(0.0)
{
  if (_r_ion < 0.0)
    paramError("r_ion", "The ion reflection coefficient cannot be negative.");
  if ((_flux_model == "bohm" || _flux_model == "current_normalized_bohm") &&
      (!_electrons || !_electron_energy))
    paramError("electrons",
               "electrons and electron_energy are required for a Bohm target-flux model.");
}

void
TargetIonFluxSideUserObject::initialize()
{
  _incoming_flux_integral = 0.0;
  _target_area = 0.0;
  _raw_bohm_flux_integral = 0.0;
  _raw_bohm_current = 0.0;
  _raw_secondary_electron_rate = 0.0;
  _normalization_factor = 1.0;
  _ion_current = 0.0;
  _discharge_current = 0.0;
  _commanded_current_value = 0.0;
  _secondary_electron_rate = 0.0;
  _profile_radius.clear();
  _profile_raw_flux.clear();
  _profile_area.clear();
  _profile_raw_secondary_rate.clear();
}

Real
TargetIonFluxSideUserObject::bohmFlux(const unsigned int qp) const
{
  const Real mean_energy = std::exp((*_electron_energy)[qp] - (*_electrons)[qp]);
  const Real electron_temperature = 2.0 / 3.0 * mean_energy;
  const Real sound_speed =
      std::sqrt((ZAPDOS_CONSTANTS::e * electron_temperature +
                 ZAPDOS_CONSTANTS::k_boltz * _ion_temperature[qp]) /
                _mass[qp]);
  return std::exp(_ions[qp]) * sound_speed;
}

void
TargetIonFluxSideUserObject::execute()
{
  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    Real normal_flux = 0.0;
    if (_flux_model == "drift_only")
      normal_flux = _sgn[qp] * _mu[qp].value() * raw_value(_electric_field[qp]) * _r_units *
                    std::exp(_ions[qp]) * _normals[qp];
    else if (_flux_model == "secondary_electron_bc")
    {
      const Real drift_to_wall =
          _normals[qp] * _sgn[qp] * raw_value(_electric_field[qp]) > 0.0 ? 1.0 : 0.0;
      const Real ion_thermal_velocity =
          std::sqrt(8.0 * ZAPDOS_CONSTANTS::k_boltz * _ion_temperature[qp] /
                    (libMesh::pi * _mass[qp]));

      normal_flux = std::exp(_ions[qp]) * (1.0 - _r_ion) / (1.0 + _r_ion) *
                    (0.5 * ion_thermal_velocity +
                     (2.0 * drift_to_wall - 1.0) * _sgn[qp] * _mu[qp].value() *
                         raw_value(_electric_field[qp]) * _r_units * _normals[qp]);
    }
    else if (_flux_model == "bohm" || _flux_model == "current_normalized_bohm")
      normal_flux = bohmFlux(qp);
    else
      mooseError("Unhandled target ion flux model.");

    const Real incoming_flux = std::max(normal_flux, 0.0);
    const Real mesh_weight = _JxW[qp] * _coord[qp];
    const bool uses_bohm_flux =
        _flux_model == "bohm" || _flux_model == "current_normalized_bohm";
    const Real weight = uses_bohm_flux
                            ? mesh_weight * _position_units * _position_units
                            : mesh_weight;

    _incoming_flux_integral += weight * incoming_flux;
    _target_area += weight;

    if (uses_bohm_flux)
    {
      const Real gamma = std::max(_secondary_emission.value(_t, _q_point[qp]), 0.0);
      const Real charge_per_density_unit =
          ZAPDOS_CONSTANTS::e * (_use_moles ? ZAPDOS_CONSTANTS::N_A : 1.0);
      _raw_bohm_flux_integral += weight * incoming_flux;
      _raw_bohm_current += weight * charge_per_density_unit * (1.0 + gamma) * incoming_flux;
      _raw_secondary_electron_rate += weight * gamma * incoming_flux;
      _profile_radius.push_back(std::abs(_q_point[qp](0)));
      _profile_raw_flux.push_back(incoming_flux);
      _profile_area.push_back(weight);
      _profile_raw_secondary_rate.push_back(weight * gamma * incoming_flux);
    }
  }
}

void
TargetIonFluxSideUserObject::finalize()
{
  gatherSum(_incoming_flux_integral);
  gatherSum(_target_area);
  gatherSum(_raw_bohm_flux_integral);
  gatherSum(_raw_bohm_current);
  gatherSum(_raw_secondary_electron_rate);

  if (_flux_model == "bohm" || _flux_model == "current_normalized_bohm")
  {
    _communicator.allgather(_profile_radius, false);
    _communicator.allgather(_profile_raw_flux, false);
    _communicator.allgather(_profile_area, false);
    _communicator.allgather(_profile_raw_secondary_rate, false);
    consolidateRadialProfile();

    if (_flux_model == "current_normalized_bohm")
    {
      _commanded_current_value = std::max(_commanded_current.value(_t, Point()), 0.0);
      _normalization_factor =
          std::min(_max_normalization,
                   _commanded_current_value / std::max(_raw_bohm_current, _current_floor));
    }
    _incoming_flux_integral = _normalization_factor * _raw_bohm_flux_integral;
    const Real charge_per_density_unit =
        ZAPDOS_CONSTANTS::e * (_use_moles ? ZAPDOS_CONSTANTS::N_A : 1.0);
    _ion_current = charge_per_density_unit * _incoming_flux_integral;
    _discharge_current = _normalization_factor * _raw_bohm_current;
    _secondary_electron_rate =
        _normalization_factor * _raw_secondary_electron_rate;
  }
}

void
TargetIonFluxSideUserObject::threadJoin(const UserObject & y)
{
  const TargetIonFluxSideUserObject & uo = dynamic_cast<const TargetIonFluxSideUserObject &>(y);
  _incoming_flux_integral += uo._incoming_flux_integral;
  _target_area += uo._target_area;
  _raw_bohm_flux_integral += uo._raw_bohm_flux_integral;
  _raw_bohm_current += uo._raw_bohm_current;
  _raw_secondary_electron_rate += uo._raw_secondary_electron_rate;
  _profile_radius.insert(
      _profile_radius.end(), uo._profile_radius.begin(), uo._profile_radius.end());
  _profile_raw_flux.insert(
      _profile_raw_flux.end(), uo._profile_raw_flux.begin(), uo._profile_raw_flux.end());
  _profile_area.insert(_profile_area.end(), uo._profile_area.begin(), uo._profile_area.end());
  _profile_raw_secondary_rate.insert(_profile_raw_secondary_rate.end(),
                                     uo._profile_raw_secondary_rate.begin(),
                                     uo._profile_raw_secondary_rate.end());
}

Real
TargetIonFluxSideUserObject::incomingFluxAverage() const
{
  return _target_area > 0.0 ? _incoming_flux_integral / _target_area : 0.0;
}

Real
TargetIonFluxSideUserObject::rawBohmFluxAverage() const
{
  return _target_area > 0.0 ? _raw_bohm_flux_integral / _target_area : 0.0;
}

void
TargetIonFluxSideUserObject::consolidateRadialProfile()
{
  if (_profile_radius.size() != _profile_raw_flux.size() ||
      _profile_radius.size() != _profile_area.size() ||
      _profile_radius.size() != _profile_raw_secondary_rate.size())
    mooseError("Target ion-flux radial profile arrays have inconsistent sizes.");

  std::vector<std::size_t> order(_profile_radius.size());
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [this](const std::size_t a, const std::size_t b) {
    return _profile_radius[a] < _profile_radius[b];
  });

  std::vector<Real> radii;
  std::vector<Real> area_weighted_fluxes;
  std::vector<Real> areas;
  std::vector<Real> secondary_rates;
  for (const auto index : order)
  {
    const Real radius = _profile_radius[index];
    const Real flux = _profile_raw_flux[index];
    const Real area = _profile_area[index];
    const Real secondary_rate = _profile_raw_secondary_rate[index];
    const Real tolerance = 1e-12 * std::max(1.0, std::abs(radius));
    if (!radii.empty() && std::abs(radius - radii.back()) <= tolerance)
    {
      area_weighted_fluxes.back() += area * flux;
      areas.back() += area;
      secondary_rates.back() += secondary_rate;
    }
    else
    {
      radii.push_back(radius);
      area_weighted_fluxes.push_back(area * flux);
      areas.push_back(area);
      secondary_rates.push_back(secondary_rate);
    }
  }
  for (std::size_t i = 0; i < area_weighted_fluxes.size(); ++i)
  {
    if (areas[i] <= 0.0)
      mooseError("Target ion-flux annulus has nonpositive physical area.");
    area_weighted_fluxes[i] /= areas[i];
  }

  _profile_radius.swap(radii);
  _profile_raw_flux.swap(area_weighted_fluxes);
  _profile_area.swap(areas);
  _profile_raw_secondary_rate.swap(secondary_rates);
}

std::size_t
TargetIonFluxSideUserObject::annulusIndex(Real radius) const
{
  if (_profile_radius.empty())
    mooseError("The target ion-flux annular profile is empty.");

  radius = std::abs(radius);
  const auto upper = std::lower_bound(_profile_radius.begin(), _profile_radius.end(), radius);
  if (upper == _profile_radius.begin())
    return 0;
  if (upper == _profile_radius.end())
    return _profile_radius.size() - 1;

  const std::size_t upper_index = std::distance(_profile_radius.begin(), upper);
  const std::size_t lower_index = upper_index - 1;
  return radius - _profile_radius[lower_index] <= _profile_radius[upper_index] - radius
             ? lower_index
             : upper_index;
}

Real
TargetIonFluxSideUserObject::normalizedSecondaryElectronRate(const std::size_t annulus) const
{
  if (annulus >= _profile_raw_secondary_rate.size())
    mooseError("Requested target ion-flux annulus index is out of range.");
  return _normalization_factor * _profile_raw_secondary_rate[annulus];
}

Real
TargetIonFluxSideUserObject::normalizedFluxAtRadius(Real radius) const
{
  if (_profile_radius.empty())
    return incomingFluxAverage();

  radius = std::abs(radius);
  if (radius <= _profile_radius.front())
    return _normalization_factor * _profile_raw_flux.front();
  if (radius >= _profile_radius.back())
    return _normalization_factor * _profile_raw_flux.back();

  const auto upper = std::upper_bound(_profile_radius.begin(), _profile_radius.end(), radius);
  const std::size_t upper_index = std::distance(_profile_radius.begin(), upper);
  const std::size_t lower_index = upper_index - 1;
  const Real fraction = (radius - _profile_radius[lower_index]) /
                        (_profile_radius[upper_index] - _profile_radius[lower_index]);
  const Real raw_flux = _profile_raw_flux[lower_index] +
                        fraction * (_profile_raw_flux[upper_index] -
                                    _profile_raw_flux[lower_index]);
  return _normalization_factor * raw_flux;
}
