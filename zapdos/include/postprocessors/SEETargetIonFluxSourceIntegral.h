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

#include "ElementIntegralPostprocessor.h"

class Function;
class SEETargetIonFluxDepositionUserObject;
class SEETargetIonFluxResponseUserObject;
class TargetIonFluxSideUserObject;

/**
 * Integrates rates produced by the actual, prescribed, or lagged SEE target ion-flux model.
 */
class SEETargetIonFluxSourceIntegral : public ElementIntegralPostprocessor
{
public:
  static InputParameters validParams();

  SEETargetIonFluxSourceIntegral(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

private:
  Real spatialWeight() const;
  Real positivePart(const Real value) const;
  Real neutralLimiter() const;
  Real sheathVoltage() const;
  Real ionizationYield() const;
  Real secondaryElectronVolumeRate() const;

  const MooseEnum _value_type;
  const TargetIonFluxSideUserObject * const _target_ion_flux;
  const Function * const _prescribed_target_ion_flux;
  const Function * const _spatial_weight_function;
  const SEETargetIonFluxResponseUserObject * const _target_ion_flux_response;
  const SEETargetIonFluxDepositionUserObject * const _conservative_deposition;
  const bool _has_potential;
  const VariableValue * const _potential;
  const bool _use_neutral_limiter;
  const VariableValue * const _neutral_density;
  const VariableValue * const _ion_density;
  const Function & _sheath_voltage;
  const Function & _electrode_voltage;
  const Function & _secondary_emission;

  const MooseEnum _target_location;
  const bool _use_target_flux_profile;
  const MooseEnum _radial_profile;
  const Real _axial_decay_length;
  const Real _target_radius;
  const Real _target_edge_width;
  const Real _target_radial_center;
  const Real _target_radial_width;
  const bool _normalize_annular_profile;
  Real _annular_normalization;
  const Real _ionization_energy;
  const Real _ionization_efficiency;
  const Real _max_ionizations_per_secondary;
  const Real _bulk_energy_per_pair;
  const Real _emission_energy;
  const Real _sheath_energy_absorption_fraction;
  const bool _subtract_ionization_energy_from_absorbed_energy;
  const Real _neutral_limiter_floor;
};
