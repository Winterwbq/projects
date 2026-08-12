//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details

#pragma once

#include "ElementUserObject.h"

#include <vector>

class TargetIonFluxSideUserObject;

/**
 * Conservatively distributes current-normalized target SEE rates into the volume.
 */
class SEETargetIonFluxDepositionUserObject : public ElementUserObject
{
public:
  static InputParameters validParams();

  SEETargetIonFluxDepositionUserObject(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;

  Real secondaryElectronVolumeRate(const Point & point) const;

protected:
  Real axialWeight(const Point & point) const;
  Real physicalVolumeScale() const;

  const TargetIonFluxSideUserObject & _target_ion_flux;
  const MooseEnum _target_location;
  const Real _axial_decay_length;
  const Real _position_units;

  std::vector<Real> _annulus_weighted_volumes;
};
