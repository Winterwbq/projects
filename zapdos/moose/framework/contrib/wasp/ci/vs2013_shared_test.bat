SET SRC_DIR=%cd%
SET BLD_DIR=%SRC_DIR%\build
dir %BLD_DIR%
if %errorlevel% == 0 rd /s /q %BLD_DIR%
mkdir "%BLD_DIR%"
cd %BLD_DIR%

REM Put the Miniconda3 installation somewhere
SET "MINI_ROOT=%BLD_DIR%\miniconda3"
curl https://code-int.ornl.gov/lefebvre/miniconda/-/raw/main/Miniconda3-py310_24.5.0-0-Windows-x86_64.exe -o Miniconda3-latest-Windows-x86_64.exe
Miniconda3-latest-Windows-x86_64.exe /S /D=%MINI_ROOT%
CALL %cd%\miniconda3\Scripts\activate.bat
set CONDA_NUMBER_CHANNEL_NOTICES=0
CALL conda env create -f ..\ci\env.yml
CALL conda activate wasp_ci
for /f "delims=" %%i in ('where python') do set PYTHON_exe=%%i

cmake -DBUILD_SHARED_LIBS=ON ^
      -DBUILDNAME="VS2013-Shared-Release-%CI_COMMIT_REF_NAME%" ^
      -DCMAKE_BUILD_TYPE:STRING=RELEASE ^
      -DPython3_EXECUTABLE=%PYTHON_exe% ^
      -Dwasp_ENABLE_SWIG=ON ^
      -Dwasp_ENABLE_TESTS:BOOL=ON ^
      -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON ^
      -G "Visual Studio 12 2013 Win64" ..

ctest -VV --output-on-failure ^
      -D ExperimentalStart ^
      -D ExperimentalBuild ^
      -D ExperimentalTest ^
      -D ExperimentalSubmit
