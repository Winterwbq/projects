REM Set up the basic directories that will be used
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
CALL pip install delvewheel

REM Hardcoded because CMake/TriBITS/warroom13/cmd isn't playing nicely
REM SET PYTHON_EXE=%MINI_ROOT%/envs/wasp_ci/bin/python
REM call ci\sanitize_path_to_python.bat
REM echo Python set to: %PYTHON_EXE%
cmake -DBUILD_SHARED_LIBS=ON ^
      -DBUILDNAME="VS2022-Shared-Release-%CI_COMMIT_REF_NAME%" ^
      -DCMAKE_BUILD_TYPE:STRING=RELEASE ^
      -DWASP_ENABLE_SWIG=ON ^
      -Dwasp_ENABLE_TESTS:BOOL=ON ^
      -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON ^
      -DCMAKE_CXX_FLAGS="/wd4005 /wd4244 /wd4251 /wd4267 /EHsc" ^
      -G "Visual Studio 17 2022" %SRC_DIR%


set CMAKE_BUILD_PARALLEL_LEVEL=28


ctest -VV --output-on-failure ^
      --build-config RELEASE ^
      -D ExperimentalStart ^
      -D ExperimentalBuild ^
      -D ExperimentalTest 

REM This command will "repair" the wheel and overwrite the existing wheel.
FOR /F "usebackq delims=" %%g IN (`WHERE /R %BLD_DIR%\wasppy\dist\ *.whl`) DO delvewheel repair -w %BLD_DIR%\wasppy\dist %%g
