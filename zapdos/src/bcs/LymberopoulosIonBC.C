//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LymberopoulosIonBC.h"

#include <cmath>

registerMooseObject("ZapdosApp", LymberopoulosIonBC);

InputParameters
LymberopoulosIonBC::validParams()
{
  InputParameters params = ADIntegratedBC::validParams();
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addRangeCheckedParam<Real>(
      "loss_scale", 1.0, "loss_scale >= 0", "Multiplier applied to the boundary particle loss.");
  params.addRangeCheckedParam<Real>(
      "loss_ramp_time",
      0.0,
      "loss_ramp_time >= 0",
      "If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).");
  params.addClassDescription("Simpified kinetic ion boundary condition"
                             " (Based on [!cite](Lymberopoulos1993))");
  return params;
}

LymberopoulosIonBC::LymberopoulosIonBC(const InputParameters & parameters)
  : ADIntegratedBC(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _loss_scale(getParam<Real>("loss_scale")),
    _loss_ramp_time(getParam<Real>("loss_ramp_time")),

    _electric_field(
        getADMaterialProperty<RealVectorValue>(getParam<std::string>("field_property_name"))),

    _mu(getADMaterialProperty<Real>("mu" + _var.name()))
{
}

ADReal
LymberopoulosIonBC::computeQpResidual()
{
  using std::exp;
  Real loss_factor = _loss_scale;
  if (_loss_ramp_time > 0.0)
    loss_factor *= std::tanh(_t / _loss_ramp_time);

  return loss_factor * _test[_i][_qp] * _r_units * _mu[_qp] * _electric_field[_qp] * _r_units *
         exp(_u[_qp]) * _normals[_qp];
}
