//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SecondaryElectronBC.h"
#include "Function.h"
#include "Zapdos.h"
#include "MagnetizedTransportTensor.h"

#include <cmath>

registerADMooseObject("ZapdosApp", SecondaryElectronBC);

namespace
{
ADReal
secondaryElectronActualMeanEnergy(const ADReal & log_actual_mean_energy,
                                  const bool clamp,
                                  const Real min_value,
                                  const Real max_value)
{
  const Real value = std::exp(log_actual_mean_energy.value());
  ADReal actual_mean_energy;
  if (!clamp)
  {
    actual_mean_energy.value() = value;
    actual_mean_energy.derivatives() = value * log_actual_mean_energy.derivatives();
  }
  else if (!std::isfinite(value) || value < min_value)
  {
    actual_mean_energy.value() = min_value;
    actual_mean_energy.derivatives() = 0.0;
  }
  else if (value > max_value)
  {
    actual_mean_energy.value() = max_value;
    actual_mean_energy.derivatives() = 0.0;
  }
  else
  {
    actual_mean_energy.value() = value;
    actual_mean_energy.derivatives() = value * log_actual_mean_energy.derivatives();
  }
  return actual_mean_energy;
}
}

InputParameters
SecondaryElectronBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredParam<Real>("r", "The reflection coefficient of the electrons.");
  params.addParam<Real>("r_ion", 0, "The reflection coefficient of the ions.");
  params.addRequiredCoupledVar("electron_energy", "The mean electron energy density in log form");
  params.addRequiredCoupledVar("ions", "A list of ion densities in log form");
  params.addCoupledVar("ion_temperatures", 300, "A list of ion temperatures in Kelvin.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addParam<bool>(
      "use_magnetized_transport", false, "Use magnetized electron drift velocity at the wall.");
  params.addParam<FunctionName>("magnetic_field_r", "0", "Radial magnetic field Br in Tesla.");
  params.addParam<FunctionName>("magnetic_field_z", "0", "Axial magnetic field Bz in Tesla.");
  params.addRangeCheckedParam<Real>(
      "emission_drift_floor_fraction",
      0.0,
      "emission_drift_floor_fraction >= 0",
      "Minimum electron drift speed used in the secondary electron density denominator, as a "
      "fraction of the local electron thermal speed. A positive value regularizes startup when "
      "the sheath field is near zero.");
  params.addParam<bool>("clamp_actual_mean_energy",
                        false,
                        "Clamp the electron mean energy used in the thermal speed calculation.");
  params.addRangeCheckedParam<Real>(
      "actual_mean_energy_min", 0.01, "actual_mean_energy_min > 0", "Minimum mean energy in eV.");
  params.addRangeCheckedParam<Real>(
      "actual_mean_energy_max", 100.0, "actual_mean_energy_max > 0", "Maximum mean energy in eV.");
  params.addRequiredParam<std::vector<std::string>>(
      "emission_coeffs", "A list of species-dependent secondary electron emission coefficients");
  params.addClassDescription("Kinetic secondary electron boundary condition");
  return params;
}

SecondaryElectronBC::SecondaryElectronBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),
    _r_ion(getParam<Real>("r_ion")),
    _emission_drift_floor_fraction(getParam<Real>("emission_drift_floor_fraction")),
    _clamp_actual_mean_energy(getParam<bool>("clamp_actual_mean_energy")),
    _actual_mean_energy_min(getParam<Real>("actual_mean_energy_min")),
    _actual_mean_energy_max(getParam<Real>("actual_mean_energy_max")),
    _num_ions(coupledComponents("ions")),
    _se_coeff_names(getParam<std::vector<std::string>>("emission_coeffs")),
    // Coupled Variables
    _mean_en(adCoupledValue("electron_energy")),

    _muem(getADMaterialProperty<Real>("mu" + _var.name())),
    _massem(getMaterialProperty<Real>("mass" + _var.name())),

    _electric_field(
        getADMaterialProperty<RealVectorValue>(getParam<std::string>("field_property_name"))),
    _use_magnetized_transport(getParam<bool>("use_magnetized_transport")),
    _magnetic_field_r(_use_magnetized_transport ? &getFunction("magnetic_field_r") : nullptr),
    _magnetic_field_z(_use_magnetized_transport ? &getFunction("magnetic_field_z") : nullptr)

{
  if (_actual_mean_energy_min >= _actual_mean_energy_max)
    mooseError(name(),
               ": actual_mean_energy_min must be smaller than actual_mean_energy_max in ",
               type());
  _ion_flux = 0;
  _a = 0.5;
  _b = 0.5;
  _v_thermal = 0.0;
  _n_gamma = 0.0;

  if (_se_coeff_names.size() != _num_ions)
    mooseError("SecondaryElectronBC with name ",
               name(),
               ": The lengths of `ions` and `emission_coeffs` must be the same");
  // Resize the vectors to store _num_ions values:
  _ip.resize(_num_ions);
  _grad_ip.resize(_num_ions);
  _muip.resize(_num_ions);
  _Tip.resize(_num_ions);
  _massip.resize(_num_ions);
  _sgnip.resize(_num_ions);
  _se_coeff.resize(_num_ions);

  // Retrieve the values for each ion and store in the relevant vectors.
  // Note that these need to be dereferenced to get the values inside the
  // main body of the code.
  // e.g. instead of "_ip[_qp]" it would be "(*_ip[i])[_qp]", where "i"
  // refers to a single ion species.
  for (unsigned int i = 0; i < _num_ions; ++i)
  {
    _ip[i] = &adCoupledValue("ions", i);
    _grad_ip[i] = &adCoupledGradient("ions", i);
    _muip[i] = &getADMaterialProperty<Real>("mu" + (*getVar("ions", i)).name());
    _Tip[i] = &adCoupledValue("ion_temperatures", i);
    _massip[i] = &getMaterialProperty<Real>("mass" + (*getVar("ions", i)).name());
    _sgnip[i] = &getMaterialProperty<Real>("sgn" + (*getVar("ions", i)).name());
    _se_coeff[i] = &getADMaterialProperty<Real>(_se_coeff_names[i]);
  }
}

ADReal
SecondaryElectronBC::computeQpResidual()
{
  using std::exp;
  using std::sqrt;
  ADRealVectorValue electron_drift_velocity = -_muem[_qp] * _electric_field[_qp] * _r_units;
  if (_use_magnetized_transport)
  {
    const RealVectorValue magnetic_field(_magnetic_field_r->value(_t, _q_point[_qp]),
                                         _magnetic_field_z->value(_t, _q_point[_qp]),
                                         0.0);
    electron_drift_velocity =
        Zapdos::magnetizedTransportTensor(electron_drift_velocity, magnetic_field, -_muem[_qp]);
  }
  const ADReal electron_drift_normal = electron_drift_velocity * _normals[_qp];

  if (electron_drift_normal > 0.0)
  {
    _a = 1.0;
  }
  else
  {
    _a = 0.0;
  }

  _ion_flux = 0;
  for (unsigned int i = 0; i < _num_ions; ++i)
  {
    using std::exp;
    using std::sqrt;
    if (_normals[_qp] * (*_sgnip[i])[_qp] * _electric_field[_qp] > 0.0)
      _b = 1.0;
    else
      _b = 0.0;
    _ion_flux += (*_se_coeff[i])[_qp] * exp((*_ip[i])[_qp]) *
                 (0.5 * sqrt(8 * ZAPDOS_CONSTANTS::k_boltz * (*_Tip[i])[_qp] /
                             (libMesh::pi * (*_massip[i])[_qp])) +
                  (2 * _b - 1) * (*_sgnip[i])[_qp] * (*_muip[i])[_qp] * _electric_field[_qp] *
                      _r_units * _normals[_qp]);
  }
  _ion_flux *= (1.0 - _r_ion) / (1.0 + _r_ion);

  const ADReal actual_mean_energy = secondaryElectronActualMeanEnergy(_mean_en[_qp] - _u[_qp],
                                                                      _clamp_actual_mean_energy,
                                                                      _actual_mean_energy_min,
                                                                      _actual_mean_energy_max);
  _v_thermal = sqrt(8 * ZAPDOS_CONSTANTS::e * 2.0 / 3 * actual_mean_energy /
                    (libMesh::pi * _massem[_qp]));
  const ADReal emission_drift_denominator =
      _emission_drift_floor_fraction > 0.0
          ? sqrt((-electron_drift_normal) * (-electron_drift_normal) +
                 (_emission_drift_floor_fraction * _v_thermal) *
                     (_emission_drift_floor_fraction * _v_thermal))
          : -electron_drift_normal + std::numeric_limits<double>::epsilon();

  _n_gamma = (1. - _a) * _ion_flux / emission_drift_denominator;

  return _test[_i][_qp] * _r_units *
         ((1. - _r) / (1. + _r) * (-0.5 * _v_thermal * _n_gamma) -
          2. / (1. + _r) * (1. - _a) * _ion_flux);
}
