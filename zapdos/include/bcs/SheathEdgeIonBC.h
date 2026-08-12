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

class TargetIonFluxSideUserObject;

/**
 * Bohm-like ion loss at a sheath edge.
 *
 * This boundary condition intentionally does not use the local normal electric field. It is meant
 * for plasma-domain boundaries that represent the sheath edge rather than the metal surface.
 */
class SheathEdgeIonBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  SheathEdgeIonBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  /// Scaling units for the position.
  const Real _r_units;
  /// Multiplier for sheath-edge ion collection.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;
  /// Optional multiplier for the Bohm speed.
  const Real _sonic_coefficient;
  /// Ion temperature in Kelvin.
  const Real _ion_temperature;
  /// Optional shared current-normalization model.
  const TargetIonFluxSideUserObject * const _target_ion_flux;

  /// Electron density in log form.
  const ADVariableValue & _electrons;
  /// Electron energy density in log form.
  const ADVariableValue & _electron_energy;
  /// Mass of the ion species.
  const MaterialProperty<Real> & _mass;
};
