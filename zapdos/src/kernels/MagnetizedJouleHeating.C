//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MagnetizedJouleHeating.h"
#include "Function.h"
#include "MagnetizedTransportTensor.h"

registerADMooseObject("ZapdosApp", MagnetizedJouleHeating);

InputParameters
MagnetizedJouleHeating::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addRequiredCoupledVar("electrons", "The electron density in log form");
  params.addRequiredParam<std::string>("potential_units", "The potential units.");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  params.addParam<FunctionName>("magnetic_field_r", "0", "Radial magnetic field Br in Tesla.");
  params.addParam<FunctionName>("magnetic_field_z", "0", "Axial magnetic field Bz in Tesla.");
  params.addParam<std::string>("field_property_name",
                               "field_solver_interface_property",
                               "Name of the solver interface material property.");
  params.addClassDescription("Joule heating term using a prescribed magnetic field.");
  return params;
}

MagnetizedJouleHeating::MagnetizedJouleHeating(const InputParameters & parameters)
  : ADKernel(parameters),
    _r_units(1. / getParam<Real>("position_units")),
    _potential_units(getParam<std::string>("potential_units")),
    _diff(getADMaterialProperty<Real>("diff" + (*getVar("electrons", 0)).name())),
    _mu(getADMaterialProperty<Real>("mu" + (*getVar("electrons", 0)).name())),
    _sign(getMaterialProperty<Real>("sgn" + (*getVar("electrons", 0)).name())),
    _electric_field(
        getADMaterialProperty<RealVectorValue>(getParam<std::string>("field_property_name"))),
    _magnetic_field_r(getFunction("magnetic_field_r")),
    _magnetic_field_z(getFunction("magnetic_field_z")),
    _em(adCoupledValue("electrons")),
    _grad_em(adCoupledGradient("electrons"))
{
  if (_potential_units.compare("V") == 0)
    _voltage_scaling = 1.;
  else if (_potential_units.compare("kV") == 0)
    _voltage_scaling = 1000.;
  else
    mooseError("Potential units " + _potential_units + " not valid! Use V or kV.");
}

ADRealVectorValue
MagnetizedJouleHeating::applyMagnetizedTensor(const ADRealVectorValue & flux) const
{
  const RealVectorValue magnetic_field(_magnetic_field_r.value(_t, _q_point[_qp]),
                                       _magnetic_field_z.value(_t, _q_point[_qp]),
                                       0.0);
  return Zapdos::magnetizedTransportTensor(flux, magnetic_field, _sign[_qp] * _mu[_qp]);
}

ADReal
MagnetizedJouleHeating::computeQpResidual()
{
  using std::exp;
  const ADRealVectorValue unmagnetized_electron_flux =
      (_sign[_qp] * _mu[_qp] * _electric_field[_qp] * _r_units -
       _diff[_qp] * _grad_em[_qp] * _r_units) *
      exp(_em[_qp]);

  return _test[_i][_qp] * _electric_field[_qp] * _r_units * _voltage_scaling *
         applyMagnetizedTensor(unmagnetized_electron_flux);
}
