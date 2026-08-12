//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HagelaarElectronBC.h"
#include "Function.h"
#include "Zapdos.h"
#include "MagnetizedTransportTensor.h"

#include <cmath>

registerADMooseObject("ZapdosApp", HagelaarElectronBC);

namespace
{
ADReal
hagelaarElectronActualMeanEnergy(const ADReal & log_actual_mean_energy,
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
HagelaarElectronBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("electron_energy", "The mean electron energy density in log form");

  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addParam<bool>(
      "use_magnetized_transport", false, "Use magnetized electron drift velocity at the wall.");
  params.addParam<FunctionName>("magnetic_field_r", "0", "Radial magnetic field Br in Tesla.");
  params.addParam<FunctionName>("magnetic_field_z", "0", "Axial magnetic field Bz in Tesla.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier applied to the boundary particle loss.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).");
  params.addParam<bool>("clamp_actual_mean_energy",
                        false,
                        "Clamp the electron mean energy used in the thermal speed calculation.");
  params.addRangeCheckedParam<Real>(
      "actual_mean_energy_min", 0.01, "actual_mean_energy_min > 0", "Minimum mean energy in eV.");
  params.addRangeCheckedParam<Real>(
      "actual_mean_energy_max", 100.0, "actual_mean_energy_max > 0", "Maximum mean energy in eV.");
  params.addClassDescription("Kinetic electron boundary condition"
                             " (Based on [!cite](hagelaar2000boundary))");
  return params;
}

HagelaarElectronBC::HagelaarElectronBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),
    _clamp_actual_mean_energy(getParam<bool>("clamp_actual_mean_energy")),
    _actual_mean_energy_min(getParam<Real>("actual_mean_energy_min")),
    _actual_mean_energy_max(getParam<Real>("actual_mean_energy_max")),

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
  _a = 0.0;
  _v_thermal = 0.0;
}

ADReal
HagelaarElectronBC::computeQpResidual()
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

  const ADReal actual_mean_energy = hagelaarElectronActualMeanEnergy(_mean_en[_qp] - _u[_qp],
                                                                     _clamp_actual_mean_energy,
                                                                     _actual_mean_energy_min,
                                                                     _actual_mean_energy_max);
  _v_thermal = sqrt(8 * ZAPDOS_CONSTANTS::e * 2.0 / 3 * actual_mean_energy /
                    (libMesh::pi * _massem[_qp]));

  Real loss_factor = _loss_scale;
  if (_loss_ramp_time > 0.0)
    loss_factor *= std::tanh(_t / _loss_ramp_time);

  return loss_factor * _test[_i][_qp] * _r_units * (1. - _r) / (1. + _r) *
         ((2 * _a - 1) * electron_drift_normal * exp(_u[_qp]) +
          0.5 * _v_thermal * exp(_u[_qp]));
}
