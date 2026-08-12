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

#include "GeneralPostprocessor.h"

class SEETargetIonFluxResponseUserObject;

/**
 * Reports the state and limiting diagnostics of the SEE target-flux response.
 */
class SEETargetIonFluxResponsePostprocessor : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  SEETargetIonFluxResponsePostprocessor(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override {}
  virtual Real getValue() const override;

protected:
  const MooseEnum _value_type;
  const SEETargetIonFluxResponseUserObject & _target_ion_flux_response;
};
