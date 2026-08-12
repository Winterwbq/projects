#include "MagnetizedTransportTensor.h"

#include <cassert>
#include <cmath>

namespace
{
struct Vec3
{
  double values[3];

  double & operator()(const unsigned int i) { return values[i]; }
  double operator()(const unsigned int i) const { return values[i]; }
};

struct FieldVec3
{
  double values[3];

  double operator()(const unsigned int i) const { return values[i]; }
};

void
expectNear(const double actual, const double expected)
{
  assert(std::abs(actual - expected) < 1e-12);
}
}

int
main()
{
  const Vec3 flux{{1.0, 0.0, 0.0}};

  const FieldVec3 axial_field{{0.0, 1.0, 0.0}};
  const auto axial_flux = Zapdos::magnetizedTransportTensor(flux, axial_field, -2.0);
  expectNear(axial_flux(0), 0.2);
  expectNear(axial_flux(1), 0.0);
  expectNear(axial_flux(2), 0.0);

  const FieldVec3 radial_field{{1.0, 0.0, 0.0}};
  const auto radial_flux = Zapdos::magnetizedTransportTensor(flux, radial_field, -2.0);
  expectNear(radial_flux(0), 1.0);
  expectNear(radial_flux(1), 0.0);
  expectNear(radial_flux(2), 0.0);

  return 0;
}
