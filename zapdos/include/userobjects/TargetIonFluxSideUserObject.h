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

#include "SideUserObject.h"

#include <cstddef>
#include <vector>

class Function;

/**
 * Computes the incoming positive-ion flux to a target boundary.
 */
class TargetIonFluxSideUserObject : public SideUserObject
{
public:
  static InputParameters validParams();

  TargetIonFluxSideUserObject(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;

  const Real & incomingFluxIntegral() const { return _incoming_flux_integral; }
  Real incomingFluxAverage() const;
  const Real & targetArea() const { return _target_area; }
  Real rawBohmFluxAverage() const;
  const Real & rawBohmCurrent() const { return _raw_bohm_current; }
  const Real & normalizationFactor() const { return _normalization_factor; }
  const Real & ionCurrent() const { return _ion_current; }
  const Real & dischargeCurrent() const { return _discharge_current; }
  const Real & commandedCurrent() const { return _commanded_current_value; }
  const Real & secondaryElectronRate() const { return _secondary_electron_rate; }
  Real normalizedFluxAtRadius(Real radius) const;
  std::size_t annulusCount() const { return _profile_radius.size(); }
  std::size_t annulusIndex(Real radius) const;
  Real normalizedSecondaryElectronRate(std::size_t annulus) const;

protected:
  Real bohmFlux(unsigned int qp) const;
  void consolidateRadialProfile();

  const MooseVariable & _ion_var;
  const VariableValue & _ions;
  const VariableValue & _ion_temperature;
  const VariableValue * const _electrons;
  const VariableValue * const _electron_energy;
  const MooseEnum _flux_model;
  const Real _position_units;
  const Real _r_units;
  const Real _r_ion;
  const bool _use_moles;
  const Function & _secondary_emission;
  const Function & _commanded_current;
  const Real _current_floor;
  const Real _max_normalization;
  const ADMaterialProperty<Real> & _mu;
  const MaterialProperty<Real> & _sgn;
  const MaterialProperty<Real> & _mass;
  const ADMaterialProperty<RealVectorValue> & _electric_field;

  Real _incoming_flux_integral;
  Real _target_area;
  Real _raw_bohm_flux_integral;
  Real _raw_bohm_current;
  Real _raw_secondary_electron_rate;
  Real _normalization_factor;
  Real _ion_current;
  Real _discharge_current;
  Real _commanded_current_value;
  Real _secondary_electron_rate;
  std::vector<Real> _profile_radius;
  std::vector<Real> _profile_raw_flux;
  std::vector<Real> _profile_area;
  std::vector<Real> _profile_raw_secondary_rate;
};
