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
 * Electric-field advection term using a magnetized mobility tensor.
 *
 * Densities must be in logarithmic form. In 2D RZ, component 0 is radial, component 1 is axial,
 * and component 2 is not transported.
 */
class MagnetizedEFieldAdvection : public ADKernel
{
public:
  static InputParameters validParams();

  MagnetizedEFieldAdvection(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

private:
  ADRealVectorValue applyMagnetizedTensor(const ADRealVectorValue & flux) const;

  /// Position units
  const Real _r_units;

  /// Mobility coefficient
  const ADMaterialProperty<Real> & _mu;
  /// Charge sign of the species
  const MaterialProperty<Real> & _sign;
  /// The electric field provided as a material property
  const ADMaterialProperty<RealVectorValue> & _electric_field;
  /// Radial magnetic field in Tesla
  const Function & _magnetic_field_r;
  /// Axial magnetic field in Tesla
  const Function & _magnetic_field_z;
};
