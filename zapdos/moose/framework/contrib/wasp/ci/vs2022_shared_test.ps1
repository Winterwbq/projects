$ErrorActionPreference = "Stop"

# Clean up and recreate build dir
if (Test-Path "$CI_PROJECT_DIR/build") {
    Remove-Item -Recurse -Force "$CI_PROJECT_DIR/build"
}
New-Item -ItemType Directory -Path "$CI_PROJECT_DIR/build" | Out-Null
Set-Location "$CI_PROJECT_DIR/build"

# Set Miniconda installation path
$MINI_ROOT = Join-Path $CI_PROJECT_DIR "build/miniconda3"
$MINICONDA_INSTALLER = "Miniconda3-py310_24.5.0-0-Windows-x86_64.exe"

# Download and install Miniconda
Invoke-WebRequest -Uri "https://code-int.ornl.gov/lefebvre/miniconda/-/raw/main/$MINICONDA_INSTALLER" -OutFile $MINICONDA_INSTALLER
Start-Process -FilePath $MINICONDA_INSTALLER -ArgumentList "/S", "/D=$MINI_ROOT" -Wait

# Setup conda shell hook for current PowerShell session
& "$MINI_ROOT\condabin\conda.bat" shell.powershell hook | Out-String | Invoke-Expression
conda env create -f "..\ci\env.yml"
conda activate wasp_ci
pip install delvewheel

# Locate python and sanitize path for CMake
$PYTHON_exe = (where python)[0] -replace '\\', '/'

# Run CMake configure
cmake -S $CI_PROJECT_DIR -B $CI_PROJECT_DIR/build `
  -DBUILD_SHARED_LIBS=ON `
  -DBUILDNAME="VS2022-Shared-Release-$env:CI_COMMIT_REF_NAME" `
  -DCMAKE_BUILD_TYPE:STRING=RELEASE `
  -DPython3_EXECUTABLE="$PYTHON_exe" `
  -DWASP_ENABLE_SWIG=ON `
  -Dwasp_ENABLE_TESTS:BOOL=ON `
  -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON `
  -DCMAKE_CXX_FLAGS="/wd4005 /wd4244 /wd4251 /wd4267 /EHsc" `
  -G "Visual Studio 17 2022"

# Set parallel build
$env:CMAKE_BUILD_PARALLEL_LEVEL = 28

# Run ctest
ctest -VV --output-on-failure `
      --build-config RELEASE `
      -D ExperimentalStart `
      -D ExperimentalBuild `
      -D ExperimentalTest `
      -D ExperimentalSubmit

# Repair wheels using delvewheel
$wheelPaths = Get-ChildItem -Recurse -Filter *.whl -Path "$CI_PROJECT_DIR\build\wasppy\dist" | Select-Object -ExpandProperty FullName
foreach ($wheel in $wheelPaths) {
    & delvewheel repair -w "$CI_PROJECT_DIR\build\wasppy\dist" $wheel
}
