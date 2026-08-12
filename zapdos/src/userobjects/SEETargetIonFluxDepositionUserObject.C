//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details

#include "SEETargetIonFluxDepositionUserObject.h"

#include "TargetIonFluxSideUserObject.h"

#include <algorithm>
#include <cmath>

registerMooseObject("ZapdosApp", SEETargetIonFluxDepositionUserObject);

InputParameters
SEETargetIonFluxDepositionUserObject::validParams()
{
  InputParameters params = ElementUserObject::validParams();
  params.addRequiredParam<UserObjectName>(
      "target_ion_flux",
      "Current-normalized target ion-flux user object providing exact annular SEE rates.");
  MooseEnum target_location("bottom top", "bottom");
  params.addParam<MooseEnum>(
      "target_location",
      target_location,
      "Boundary location of the emitting target. The volume weight decays away from this edge.");
  params.addRangeCheckedParam<Real>(
      "axial_decay_length",
      "axial_decay_length > 0",
      "Physical axial decay length of the secondary-electron deposition profile.");
  params.addRangeCheckedParam<Real>(
      "position_units", "position_units > 0", "Physical length represented by one mesh unit.");
  params.addClassDescription(
      "Conservatively distributes exact current-normalized target-annulus SEE rates over volume "
      "quadrature points.");
  return params;
}

SEETargetIonFluxDepositionUserObject::SEETargetIonFluxDepositionUserObject(
    const InputParameters & parameters)
  : ElementUserObject(parameters),
    _target_ion_flux(getUserObject<TargetIonFluxSideUserObject>("target_ion_flux")),
    _target_location(getParam<MooseEnum>("target_location")),
    _axial_decay_length(getParam<Real>("axial_decay_length")),
    _position_units(getParam<Real>("position_units"))
{
}

void
SEETargetIonFluxDepositionUserObject::initialize()
{
  if (_target_ion_flux.annulusCount() == 0)
    mooseError("The target SEE annular profile is empty. Execute target_ion_flux in an earlier "
               "execution_order_group than the conservative deposition user object.");

  _annulus_weighted_volumes.assign(_target_ion_flux.annulusCount(), 0.0);
}

Real
SEETargetIonFluxDepositionUserObject::axialWeight(const Point & point) const
{
  const Real domain_min = _mesh.getMinInDimension(1);
  const Real domain_max = _mesh.getMaxInDimension(1);
  const Real target_distance =
      (_target_location == "top" ? domain_max - point(1) : point(1) - domain_min) *
      _position_units;
  const Real domain_length = (domain_max - domain_min) * _position_units;
  const Real normalization =
      _axial_decay_length * (1.0 - std::exp(-domain_length / _axial_decay_length));

  return std::exp(-std::max(target_distance, 0.0) / _axial_decay_length) / normalization;
}

Real
SEETargetIonFluxDepositionUserObject::physicalVolumeScale() const
{
  if (_coord_sys == Moose::COORD_RZ)
    return std::pow(_position_units, _mesh.dimension() + 1);
  if (_coord_sys == Moose::COORD_RSPHERICAL)
    return std::pow(_position_units, 3);
  return std::pow(_position_units, _mesh.dimension());
}

void
SEETargetIonFluxDepositionUserObject::execute()
{
  const Real volume_scale = physicalVolumeScale();
  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    const std::size_t annulus = _target_ion_flux.annulusIndex(_q_point[qp](0));
    _annulus_weighted_volumes[annulus] +=
        _JxW[qp] * _coord[qp] * volume_scale * axialWeight(_q_point[qp]);
  }
}

void
SEETargetIonFluxDepositionUserObject::finalize()
{
  gatherSum(_annulus_weighted_volumes);
}

void
SEETargetIonFluxDepositionUserObject::threadJoin(const UserObject & y)
{
  const auto & uo = dynamic_cast<const SEETargetIonFluxDepositionUserObject &>(y);
  if (_annulus_weighted_volumes.size() != uo._annulus_weighted_volumes.size())
    mooseError("Conservative SEE deposition annulus arrays have inconsistent sizes.");

  for (std::size_t i = 0; i < _annulus_weighted_volumes.size(); ++i)
    _annulus_weighted_volumes[i] += uo._annulus_weighted_volumes[i];
}

Real
SEETargetIonFluxDepositionUserObject::secondaryElectronVolumeRate(const Point & point) const
{
  const std::size_t annulus = _target_ion_flux.annulusIndex(point(0));
  const Real weighted_volume = _annulus_weighted_volumes[annulus];
  if (weighted_volume <= 0.0)
    mooseError("Target SEE annulus ",
               annulus,
               " has no associated positive volume quadrature weight.");

  return _target_ion_flux.normalizedSecondaryElectronRate(annulus) * axialWeight(point) /
         weighted_volume;
}
