Windows BOLSIG+ run package for Cu vapor
========================================

Files in this folder:

  cu_siglo_bolsig_clean.txt
    Clean BOLSIG-compatible electron-Cu cross sections. This starts directly
    with ELASTIC, without the LXCat website header.

  bolsigminus_cu_windows.in
    BOLSIG- input file. It uses relative paths, so it can run on Windows.

  run_cu_bolsig.bat
    Double-click launcher or Command Prompt launcher.

What you need to add:

  bolsigminus.exe
    Download the Windows console executable and copy it into this folder.

How to run:

  1. Copy this whole windows_bolsig folder to the Windows computer.
  2. Put bolsigminus.exe inside the same folder.
  3. Double-click run_cu_bolsig.bat.

If double-click closes too fast:

  1. Open Command Prompt.
  2. cd to this folder.
  3. Run:

       run_cu_bolsig.bat

Expected output files:

  cu_bolsig_energy.dat
  cu_bolsig_en.dat
  bolsiglog.txt

Send or copy cu_bolsig_energy.dat back to:

  /Users/bingqingwang/projects/cu_pvd_hybrid/rate_coefficients_cu/

Then convert it into electron_moments.txt for Zapdos.
