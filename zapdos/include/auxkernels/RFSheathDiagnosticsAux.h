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

#include "AuxKernel.h"

class Function;

/**
 * Computes wafer RF sheath diagnostics from the sheath-edge plasma state.
 *
 * This object does not modify the plasma solution. It estimates unresolved sheath quantities such
 * as voltage drop, ion impact energy, and Child-Langmuir sheath thickness from local density,
 * electron energy, plasma potential, and electrode voltage.
 */
class RFSheathDiagnosticsAux : public AuxKernel
{
public:
  static InputParameters validParams();

  RFSheathDiagnosticsAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  Real electronTemperature() const;
  Real bohmSpeed() const;
  Real solverIonFlux() const;
  Real particleIonFlux() const;
  Real ionCurrentDensity() const;
  Real sheathVoltage() const;
  Real sheathThickness() const;
  Real ionImpactEnergy() const;
  Real ionTransitTime() const;
  Real lossFactor() const;

  /// Ion density in log form.
  const VariableValue & _ions;
  /// Electron density in log form.
  const VariableValue & _electrons;
  /// Electron energy density in log form.
  const VariableValue & _electron_energy;
  /// Plasma potential at the sheath edge.
  const VariableValue & _plasma_potential;

  /// Coupled ion variable, used to retrieve the species mass material property.
  const MooseVariable & _ion_var;
  /// Electrode voltage function behind the unresolved sheath.
  const Function & _electrode_potential;
  /// Requested diagnostic quantity.
  const MooseEnum _quantity;
  /// Scaling units for position.
  const Real _r_units;
  /// Whether the log density is molar density and must be converted to particles.
  const bool _use_moles;
  /// Multiplier for sheath-edge ion collection.
  const Real _loss_scale;
  /// If positive, smoothly ramps the boundary particle loss by tanh(t / loss_ramp_time).
  const Real _loss_ramp_time;
  /// Optional multiplier for the Bohm speed.
  const Real _sonic_coefficient;
  /// Ion temperature in Kelvin.
  const Real _ion_temperature;
  /// Positive ion charge state.
  const Real _charge_state;
  /// RF frequency in Hz.
  const Real _rf_frequency;
  /// Floor for Child-Langmuir current density.
  const Real _min_current_density;
  /// Floor for ion transit velocity.
  const Real _min_velocity;
  /// Mass of the ion species.
  const MaterialProperty<Real> & _mass;
};
