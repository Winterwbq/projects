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
 * Diffusion term using a magnetized transport tensor.
 *
 * Densities must be in logarithmic form. In 2D RZ, component 0 is radial, component 1 is axial,
 * and component 2 is not transported.
 */
class MagnetizedCoeffDiffusion : public ADKernel
{
public:
  static InputParameters validParams();

  MagnetizedCoeffDiffusion(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

private:
  ADRealVectorValue applyMagnetizedTensor(const ADRealVectorValue & flux) const;

  /// Position units
  const Real _r_units;

  /// The diffusion coefficient
  const ADMaterialProperty<Real> & _diffusivity;
  /// Mobility coefficient used to form the Hall parameter
  const ADMaterialProperty<Real> & _mu;
  /// Charge sign of the species
  const MaterialProperty<Real> & _sign;
  /// Radial magnetic field in Tesla
  const Function & _magnetic_field_r;
  /// Axial magnetic field in Tesla
  const Function & _magnetic_field_z;
};
