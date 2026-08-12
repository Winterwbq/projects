//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADKernel.h"

class Function;

/**
 * Joule heating term based on a magnetized electron flux.
 */
class MagnetizedJouleHeating : public ADKernel
{
public:
  static InputParameters validParams();

  MagnetizedJouleHeating(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

private:
  ADRealVectorValue applyMagnetizedTensor(const ADRealVectorValue & flux) const;

  /// Position units
  const Real _r_units;
  /// Potential units
  const std::string _potential_units;
  /// Scaling value for the potential
  Real _voltage_scaling;

  /// Electron diffusion coefficient
  const ADMaterialProperty<Real> & _diff;
  /// Electron mobility coefficient
  const ADMaterialProperty<Real> & _mu;
  /// Charge sign of electrons
  const MaterialProperty<Real> & _sign;
  /// The electric field provided as a material property
  const ADMaterialProperty<RealVectorValue> & _electric_field;
  /// Radial magnetic field in Tesla
  const Function & _magnetic_field_r;
  /// Axial magnetic field in Tesla
  const Function & _magnetic_field_z;

  /// Coupled electron density
  const ADVariableValue & _em;
  /// Gradient of electron density
  const ADVariableGradient & _grad_em;
};
