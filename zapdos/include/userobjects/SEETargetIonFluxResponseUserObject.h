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

#include "GeneralUserObject.h"

class Function;
class TargetIonFluxSideUserObject;

/**
 * Holds a timestep-lagged target ion flux for SEE source feedback.
 */
class SEETargetIonFluxResponseUserObject : public GeneralUserObject
{
public:
  static InputParameters validParams();

  SEETargetIonFluxResponseUserObject(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}

  const Real & rawTargetIonFlux() const { return _raw_target_ion_flux; }
  const Real & cappedTargetIonFlux() const { return _capped_target_ion_flux; }
  const Real & filteredTargetIonFlux() const { return _filtered_target_ion_flux; }
  const Real & rawDischargePower() const { return _raw_discharge_power; }
  const Real & powerCapFactor() const { return _power_cap_factor; }

protected:
  void updateRawDiagnostics();

  const TargetIonFluxSideUserObject & _target_ion_flux;
  const Function & _sheath_voltage;
  const Real _response_time;
  const Real _max_discharge_power;
  const Real _initial_target_ion_flux;

  Real _raw_target_ion_flux;
  Real _capped_target_ion_flux;
  Real _raw_discharge_power;
  Real _power_cap_factor;
  Real & _filtered_target_ion_flux;
  Real & _previous_update_time;
  bool & _response_initialized;
};
