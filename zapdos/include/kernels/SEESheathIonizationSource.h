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
 * Approximate nonlocal ionization driven by secondary electrons accelerated through a cathode
 * sheath. The source is proportional to the local ion density near the target, a prescribed
 * secondary-emission coefficient, and a simple sheath-energy ionization yield.
 */
class SEESheathIonizationSource : public ADKernel
{
public:
  static InputParameters validParams();

  SEESheathIonizationSource(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

private:
  ADReal sourceRate() const;
  ADReal secondaryElectronRate() const;
  Real spatialWeight() const;
  ADReal ionizationYield() const;
  ADReal sheathVoltage() const;
  ADReal positivePart(const ADReal & value) const;

  const MooseEnum _source_type;
  const ADVariableValue & _ions;
  const bool _has_potential;
  const ADVariableValue & _potential;
  const Function & _sheath_voltage;
  const Function & _electrode_voltage;
  const Function & _secondary_emission;

  const Real _ion_mass;
  const Real _bohm_electron_temperature;
  const Real _source_length;
  const Real _axial_decay_length;
  const Real _radial_center;
  const Real _radial_width;
  const Real _ionization_energy;
  const Real _ionization_efficiency;
  const Real _max_ionizations_per_secondary;
  const Real _bulk_energy_per_pair;
  const Real _sheath_energy_absorption_fraction;
};
