#ifndef WASP_MATH_H
#define WASP_MATH_H

/**
 * @file math.h
 * @brief Header-only math routines used in WASP 
 *
 * This header provides high-precision utility functions for converting input data
 * — such as atom fractions, weight fractions, material mass densities, and atom densities —
 * into per-nuclide atomic densities expressed in grams per barn-centimeter (g/b-cm).
 *
 * These routines are suitable for use in general materials analysis workflows.
 *
 * ## Supported Input Modes
 * The API supports input combinations commonly encountered in nuclear modeling:
 * - Atom fractions with mass density (g/cm³)
 * - Weight fractions with mass density (g/cm³)
 * - Atom fractions with atomic density (atoms/b-cm)
 * - Weight fractions with atomic density (atoms/b-cm)
 * - Per-nuclide atom densities (atoms/cm³)
 *
 * ## Key Concepts
 * - `D_i` is defined as the nuclide atomic density in units of g/b-cm.
 * - A barn is 1e-24 cm²; b-cm is thus 1e-24 cm³.
 * - Input fraction vectors are automatically normalized to 1.0.
 * - Molar masses (g/mol) are required for all conversions involving mass.
 * - All conversions use CODATA 2018 constants by default.
 *
 */


#include "waspcore/wasp_bug.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include "waspcore/decl.h"

namespace wasp
{

// ---------------------- Constants ------------------------

constexpr double avogadro    = 6.02214076e23;  // Avogadro's number (atoms/mol)
constexpr double barn_to_cm2 = 1e-24;          // 1 barn = 1e-24 cm²

// ---------------------- Utilities ------------------------

/**
 * @brief Normalize a vector of fractions (unitless)
 * @param fractions The input fraction vector (e.g., atom or weight fractions)
 */
inline WASP_PUBLIC void normalize(std::vector<double>& fractions)
{
    double sum = std::accumulate(fractions.begin(), fractions.end(), 0.0);
    wasp_require(sum > 0);
    for (double& f : fractions)
        f /= sum;
}

/**
 * @brief Compute average molar mass (𝑨̄, g/mol) from atom fractions
 * @param atom_fractions (unitless)
 * @param molar_masses (g/mol)
 * @return Average molar mass (g/mol)
 */
inline WASP_PUBLIC double
average_molar_mass(const std::vector<double>& atom_fractions,
                   const std::vector<double>& molar_masses)
{
    wasp_require(atom_fractions.size() == molar_masses.size());        
    double sum = 0.0;
    for (size_t i = 0; i < atom_fractions.size(); ++i)
        sum += atom_fractions[i] * molar_masses[i];
    return sum;
}

/**
 * @brief Convert weight fractions (unitless) to atom fractions (unitless)
 */
inline WASP_PUBLIC std::vector<double>
weight_to_atom_fraction(const std::vector<double>& weight_fractions,
                        const std::vector<double>& molar_masses)
{
    wasp_require(weight_fractions.size() == molar_masses.size());
        

    std::vector<double> atom_fractions(weight_fractions.size());
    double              denom = 0.0;
    for (size_t i = 0; i < weight_fractions.size(); ++i)
        denom += weight_fractions[i] / molar_masses[i];

    wasp_check(denom <= 0.0);

    for (size_t i = 0; i < weight_fractions.size(); ++i)
        atom_fractions[i] = (weight_fractions[i] / molar_masses[i]) / denom;

    normalize(atom_fractions);
    return atom_fractions;
}

// ---------------------- Conversion Routines ------------------------

/**
 * @brief Atom fractions + mass density → D_i (nuclide atomic density, g/b-cm)
 */
inline WASP_PUBLIC std::vector<double>
                   compute_di_from_atom_fraction_mass_density(
                       std::vector<double>        atom_fractions,
                       const std::vector<double>& molar_masses,
                       double                     mass_density_g_cm3)
{
    normalize(atom_fractions);
    double              abar = average_molar_mass(atom_fractions, molar_masses);
    std::vector<double> di(atom_fractions.size());

    for (size_t i = 0; i < di.size(); ++i)
        di[i] = atom_fractions[i] * mass_density_g_cm3 * molar_masses[i] /
                abar / barn_to_cm2;

    return di;
}

/**
 * @brief Weight fractions + mass density → D_i (nuclide atomic density, g/b-cm)
 */
inline WASP_PUBLIC std::vector<double>
                   compute_di_from_weight_fraction_mass_density(
                       std::vector<double> weight_fractions, double mass_density_g_cm3)
{
    normalize(weight_fractions);
    std::vector<double> di(weight_fractions.size());

    for (size_t i = 0; i < di.size(); ++i)
        di[i] = weight_fractions[i] * mass_density_g_cm3 / barn_to_cm2;

    return di;
}

/**
 * @brief Atom fractions + atomic density → D_i (nuclide atomic density, g/b-cm)
 */
inline WASP_PUBLIC std::vector<double>
                   compute_di_from_atom_fraction_atomic_density(
                       std::vector<double>        atom_fractions,
                       double                     atomic_density_bcm)
{
    normalize(atom_fractions);
    std::vector<double> di(atom_fractions.size());

    for (size_t i = 0; i < di.size(); ++i)
    {
        di[i] = atom_fractions[i] * atomic_density_bcm;
    }
    return di;
}

/**
 * @brief Weight fractions + atomic density → D_i (nuclide atomic density,
 * g/b-cm)
 */
inline WASP_PUBLIC std::vector<double>
                   compute_di_from_weight_fraction_atomic_density(
                       std::vector<double>        weight_fractions,
                       const std::vector<double>& molar_masses,
                       double                     atomic_density_bcm)
{
    normalize(weight_fractions);
    auto atom_fractions =
        weight_to_atom_fraction(weight_fractions, molar_masses);
    return compute_di_from_atom_fraction_atomic_density(
        atom_fractions, atomic_density_bcm);
}

/**
 * @brief Atom densities → D_i (nuclide atomic density, g/b-cm)
 */
inline WASP_PUBLIC std::vector<double>
compute_di_from_atom_density(const std::vector<double>& atom_densities,
                             const std::vector<double>& molar_masses)
{
    wasp_require(atom_densities.size() == molar_masses.size());

    std::vector<double> di(atom_densities.size());

    for (size_t i = 0; i < di.size(); ++i)
        di[i] = atom_densities[i] * molar_masses[i] / (avogadro * barn_to_cm2);

    return di;
}

/**
 * @brief Material molar mass + atom densities → Material mass density (g/cc)
 */
inline WASP_PUBLIC double compute_mass_density_from_atoms_bcm(
    const std::vector<double>& atom_densities_bcm,
    const std::vector<double>& molar_masses)
{
    wasp_check(atom_densities_bcm.size() == molar_masses.size())

    double rho = 0.0;
    for (size_t i = 0; i < atom_densities_bcm.size(); ++i) {
        rho += atom_densities_bcm[i] * molar_masses[i] * 1e24 / avogadro;
    }
    return rho;
}

}  // namespace wasp

#endif // WASP_MATH_H
