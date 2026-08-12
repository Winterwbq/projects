//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MagnetizedEFieldAdvection.h"
#include "Function.h"
#include "MagnetizedTransportTensor.h"

registerADMooseObject("ZapdosApp", MagnetizedEFieldAdvection);

InputParameters
MagnetizedEFieldAdvection::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<FunctionName>("magnetic_field_r", "0", "Radial magnetic field Br in Tesla.");
  params.addParam<FunctionName>("magnetic_field_z", "0", "Axial magnetic field Bz in Tesla.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addClassDescription("Electric field driven advection term with a prescribed magnetic "
                             "field. Densities must be in logarithmic form.");
  return params;
}

MagnetizedEFieldAdvection::MagnetizedEFieldAdvection(const InputParameters & parameters)
  : ADKernel(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _mu(getADMaterialProperty<Real>("mu" + _var.name())),
    _sign(getMaterialProperty<Real>("sgn" + _var.name())),
    _electric_field(
        getADMaterialProperty<RealVectorValue>(getParam<std::string>("field_property_name"))),
    _magnetic_field_r(getFunction("magnetic_field_r")),
    _magnetic_field_z(getFunction("magnetic_field_z"))
{
}

ADRealVectorValue
MagnetizedEFieldAdvection::applyMagnetizedTensor(const ADRealVectorValue & flux) const
{
  const RealVectorValue magnetic_field(_magnetic_field_r.value(_t, _q_point[_qp]),
                                       _magnetic_field_z.value(_t, _q_point[_qp]),
                                       0.0);
  return Zapdos::magnetizedTransportTensor(flux, magnetic_field, _sign[_qp] * _mu[_qp]);
}

ADReal
MagnetizedEFieldAdvection::computeQpResidual()
{
  using std::exp;
  const ADRealVectorValue unmagnetized_flux =
      _mu[_qp] * _sign[_qp] * exp(_u[_qp]) * _electric_field[_qp] * _r_units;
  return applyMagnetizedTensor(unmagnetized_flux) * -_grad_test[_i][_qp] * _r_units;
}
