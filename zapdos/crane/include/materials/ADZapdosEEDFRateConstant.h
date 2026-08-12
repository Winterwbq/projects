//* This file is part of Crane, an open-source
//* application for plasma chemistry and thermochemistry
//* https://github.com/lcpp-org/crane
//*
//* Crane is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADMaterial.h"
/* #include "LinearInterpolation.h" */
#include "SplineInterpolation.h"
//#include "LinearInterpolation.h"

class ADZapdosEEDFRateConstant : public ADMaterial
{
public:
  static InputParameters validParams();
  ADZapdosEEDFRateConstant(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();
  ADReal actualMeanEnergy() const;

  //std::unique_ptr<LinearInterpolation> _coefficient_interpolation;
  SplineInterpolation _coefficient_interpolation;

  Real _r_units;
  ADMaterialProperty<Real> & _rate_coefficient;
  const ADVariableValue & _em;
  const ADVariableValue & _mean_en;
  const bool _clamp_actual_mean_energy;
  Real _actual_mean_energy_min;
  Real _actual_mean_energy_max;

  using ADMaterial::_communicator;
};
