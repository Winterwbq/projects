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

#include "ADIntegratedBC.h"

/**
 *  Simpified kinetic ion boundary condition
 */
class LymberopoulosIonBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  LymberopoulosIonBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  /// Scaling units for the position
  const Real _r_units;
  /// Multiplier applied to the boundary particle loss.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;

  /// The electric field provided as a material property
  const ADMaterialProperty<RealVectorValue> & _electric_field;

  /// Mobility coefficient
  const ADMaterialProperty<Real> & _mu;
};
