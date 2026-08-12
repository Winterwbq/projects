@echo off
setlocal

cd /d "%~dp0"

if not exist bolsigminus.exe (
  echo ERROR: bolsigminus.exe was not found in this folder.
  echo Copy bolsigminus.exe here, next to run_cu_bolsig.bat.
  pause
  exit /b 1
)

if not exist cu_siglo_lxcat_bolsig.txt (
  echo ERROR: cu_siglo_lxcat_bolsig.txt was not found in this folder.
  pause
  exit /b 1
)

bolsigminus.exe bolsigminus_cu_windows.in

echo.
echo Finished. Check for:
echo   cu_bolsig_energy.dat
echo   cu_bolsig_en.dat
echo   bolsiglog.txt
echo.
pause
