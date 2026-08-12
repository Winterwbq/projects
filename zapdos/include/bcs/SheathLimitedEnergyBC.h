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
 * Electron energy loss limited by the sheath potential barrier.
 *
 * Uses the same sheath transmission factor as SheathLimitedElectronBC so electron density and
 * electron energy density are removed consistently at sheath-edge boundaries.
 */
class SheathLimitedEnergyBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  SheathLimitedEnergyBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  ADReal sheathTransmission() const;

  /// Scaling units for the position.
  const Real _r_units;
  /// Reflection coefficient for thermal wall collection.
  const Real _r;
  /// Multiplier for the boundary energy loss.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary energy loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;
  /// Multiplier for convected electron energy loss.
  const Real _energy_loss_coefficient;

  /// Electron density in log form.
  const ADVariableValue & _electrons;
  /// Plasma potential at the sheath edge.
  const ADVariableValue & _plasma_potential;
  /// Metal electrode voltage.
  const Function & _electrode_potential;
  /// Electron mass.
  const MaterialProperty<Real> & _mass;
};
