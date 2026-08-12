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

class Function;

/**
 * Electron thermal loss limited by the sheath potential barrier.
 *
 * The electrode voltage is used only to compute the sheath transmission factor; the local plasma
 * electric field is not used as the collection velocity.
 */
class SheathLimitedElectronBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  SheathLimitedElectronBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  ADReal sheathTransmission() const;

  /// Scaling units for the position.
  const Real _r_units;
  /// Reflection coefficient for thermal wall collection.
  const Real _r;
  /// Multiplier for the boundary particle loss.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;

  /// Electron energy density in log form.
  const ADVariableValue & _electron_energy;
  /// Plasma potential at the sheath edge.
  const ADVariableValue & _plasma_potential;
  /// Metal electrode voltage.
  const Function & _electrode_potential;
  /// Electron mass.
  const MaterialProperty<Real> & _mass;
};
