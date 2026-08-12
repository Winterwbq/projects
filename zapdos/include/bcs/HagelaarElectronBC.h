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
 *  Kinetic electron boundary condition
 */
class HagelaarElectronBC : public ADIntegratedBC
{
public:
  static InputParameters validParams();

  HagelaarElectronBC(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  /// Scaling units for the position
  const Real _r_units;
  /// Reflection coefficient
  const Real & _r;
  /// Multiplier applied to the boundary particle loss.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;
  /// Clamp electron mean energy used for thermal speed during nonlinear trial states
  const bool _clamp_actual_mean_energy;
  /// Lower electron mean energy bound in eV
  const Real _actual_mean_energy_min;
  /// Upper electron mean energy bound in eV
  const Real _actual_mean_energy_max;

  /// Electron mean energy density
  const ADVariableValue & _mean_en;

  /// Mobility coefficient of electrons
  const ADMaterialProperty<Real> & _muem;
  /// Mass of electrons
  const MaterialProperty<Real> & _massem;

  /// The electric field provided as a material property
  const ADMaterialProperty<RealVectorValue> & _electric_field;
  /// Whether to use the magnetized electron drift velocity at the wall
  const bool _use_magnetized_transport;
  /// Radial magnetic field in Tesla
  const Function * const _magnetic_field_r;
  /// Axial magnetic field in Tesla
  const Function * const _magnetic_field_z;

  /// Equal to 1 when the drift velocity is direct towards the wall and zero otherwise
  Real _a;
  /// Electron thermal velocity
  ADReal _v_thermal;
};
