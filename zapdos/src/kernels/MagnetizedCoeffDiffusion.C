//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MagnetizedCoeffDiffusion.h"
#include "Function.h"
#include "MagnetizedTransportTensor.h"

registerADMooseObject("ZapdosApp", MagnetizedCoeffDiffusion);

InputParameters
MagnetizedCoeffDiffusion::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<FunctionName>("magnetic_field_r", "0", "Radial magnetic field Br in Tesla.");
  params.addParam<FunctionName>("magnetic_field_z", "0", "Axial magnetic field Bz in Tesla.");
  params.addClassDescription("Diffusion term with a prescribed magnetic field. "
                             "Densities must be in logarithmic form.");
  return params;
}

MagnetizedCoeffDiffusion::MagnetizedCoeffDiffusion(const InputParameters & parameters)
  : ADKernel(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _diffusivity(getADMaterialProperty<Real>("diff" + _var.name())),
    _mu(getADMaterialProperty<Real>("mu" + _var.name())),
    _sign(getMaterialProperty<Real>("sgn" + _var.name())),
    _magnetic_field_r(getFunction("magnetic_field_r")),
    _magnetic_field_z(getFunction("magnetic_field_z"))
{
}

ADRealVectorValue
MagnetizedCoeffDiffusion::applyMagnetizedTensor(const ADRealVectorValue & flux) const
{
  const RealVectorValue magnetic_field(_magnetic_field_r.value(_t, _q_point[_qp]),
                                       _magnetic_field_z.value(_t, _q_point[_qp]),
                                       0.0);
  return Zapdos::magnetizedTransportTensor(flux, magnetic_field, _sign[_qp] * _mu[_qp]);
}

ADReal
MagnetizedCoeffDiffusion::computeQpResidual()
{
  using std::exp;
  const ADRealVectorValue unmagnetized_flux =
      -_diffusivity[_qp] * exp(_u[_qp]) * _grad_u[_qp] * _r_units;
  return applyMagnetizedTensor(unmagnetized_flux) * -_grad_test[_i][_qp] * _r_units;
}
