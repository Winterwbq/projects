#pragma once

namespace Zapdos
{
template <typename FluxVectorType, typename MagneticFieldVectorType, typename ScalarType>
FluxVectorType
magnetizedTransportTensor(const FluxVectorType & flux,
                          const MagneticFieldVectorType & magnetic_field,
                          const ScalarType & signed_mobility)
{
  FluxVectorType magnetized_flux;
  magnetized_flux(2) = 0.0;

  const auto br = magnetic_field(0);
  const auto bz = magnetic_field(1);
  const auto b_squared = br * br + bz * bz;

  if (b_squared == 0.0)
  {
    magnetized_flux(0) = flux(0);
    magnetized_flux(1) = flux(1);
    return magnetized_flux;
  }

  // Cap beta^2 at beta_max=100 (condition ratio 1e4) to prevent numerical degeneracy.
  // Physical basis: anomalous (Bohm) cross-field transport limits effective beta.
  // The cap also prevents Inf-Inf=NaN in the perpendicular-component split when
  // upstream mobility is Inf (e.g., p_gas → 0 before AuxKernel initializes).
  const auto beta_squared_raw = signed_mobility * signed_mobility * b_squared;
  const ScalarType beta_sq_max(1.0e4);
  const auto beta_squared = (beta_squared_raw < beta_sq_max) ? beta_squared_raw : beta_sq_max;
  const auto denom = 1.0 + beta_squared;
  const auto b_dot_flux = br * flux(0) + bz * flux(1);
  const auto parallel_r = br * b_dot_flux / b_squared;
  const auto parallel_z = bz * b_dot_flux / b_squared;
  const auto perpendicular_r = flux(0) - parallel_r;
  const auto perpendicular_z = flux(1) - parallel_z;

  magnetized_flux(0) = parallel_r + perpendicular_r / denom;
  magnetized_flux(1) = parallel_z + perpendicular_z / denom;

  return magnetized_flux;
}
}
