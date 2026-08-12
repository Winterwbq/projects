# Cu Rate Coefficient Workspace

This directory contains the Cu/Cu+ electron transport and reaction data used by
the hybrid MC-Zapdos model.

Zapdos currently uses these files:

- `electron_moments.txt`: columns are mean electron energy [eV],
  mobility times neutral density, and diffusion times neutral density. This is
  read by `ElectronTransportCoefficients/property_tables_file`.
- `reaction1.txt`: Cu electron-impact ionization table used by the Zapdos
  `[Reactions/Copper]` block:
  `em + Cu -> em + em + Cu+`.

The BOLSIG source data are also kept here:

- `cu_bolsig_energy.dat`: raw BOLSIG+ energy-format output.
- `cu_ionization_rate_bolsig.csv`: physical ionization rate coefficient with
  columns `Te_eV`, `k_iz_m3_s`, `mean_energy_eV`, and `E_over_N_Td`.
- `cu_excitation_*_rate_bolsig.csv`: excitation-rate tables retained for later
  electron energy loss channels.
- `cu_ionization_rate_maxwellian.csv`: older approximate Maxwellian rate
  coefficient for quick MC tests only.

Important unit convention:

`cu_ionization_rate_bolsig.csv` stores `k_iz` in m^3 / particle / s. The Zapdos
input uses `use_moles=true` and log(mol/m^3) densities, so `reaction1.txt`
stores `k_iz * N_A` in m^3 / mol / s. Regenerate the files with:

```bash
python scripts/convert_bolsig_cu.py
```

Use this cleaned BOLSIG-compatible LXCat file as the BOLSIG+ cross-section input:

- `/Users/bingqingwang/projects/cu_pvd_hybrid/data/cross_sections/cu_siglo/cu_siglo_bolsig_clean.txt`
