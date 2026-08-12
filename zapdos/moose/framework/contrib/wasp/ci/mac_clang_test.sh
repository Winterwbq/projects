mkdir build
cd build

#git clone https://code.ornl.gov/warroom/miniconda.git
curl -O https://code-int.ornl.gov/lefebvre/miniconda/-/raw/main/Miniconda3-py310_24.5.0-0-MacOSX-arm64.sh
bash ./Miniconda3-py310_24.5.0-0-MacOSX-arm64.sh -b -p ${PWD}/miniconda3
eval "$(${PWD}/miniconda3/bin/conda shell.bash hook 2> /dev/null)"
export CONDA_NUMBER_CHANNEL_NOTICES=0
conda env create -f ../ci/env.yml
conda activate wasp_ci
pip install delocate build

cmake -DBUILDNAME="$(uname -s)-Debug-${CI_COMMIT_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=DEBUG \
      -DWASP_ENABLE_SWIG=ON \
      -Dwasp_ENABLE_TESTS=ON \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      ..

export CMAKE_BUILD_PARALLEL_LEVEL=8

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild \
      -D ExperimentalTest 

# clean up prior config for the next bundle config
rm -rf CMake*
cmake -DBUILDNAME="$(uname -s)-AppleClang-8-Bundle-${CI_COMMIT_REF_NAME}" \
      -DCPACK_PACKAGE_NAME=WASP \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      -DWASP_ENABLE_SWIG=ON \
      -Dwasp_ENABLE_TESTS:BOOL=OFF \
      -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON \
      -Dwasp_ENABLE_testframework:BOOL=OFF \
      -Dwasp_ENABLE_wasppy:BOOL=ON \
      -Dwasp_ENABLE_INSTALL_CMAKE_CONFIG_FILES:BOOL=ON \
      -Dwasp_GENERATE_EXPORT_FILE_DEPENDENCIES:BOOL=ON \
      -Dwasp_ENABLE_CPACK_PACKAGING:BOOL=ON \
      ..

make -j 8 package
delocate-wheel -w ${CI_PROJECT_DIR}/build/wasppy/dist ${CI_PROJECT_DIR}/build/wasppy/dist/ornl_wasp*.whl

ls -l ./

# Copy bundle parts up to parent directory to avoid artifact
# having build directory
cp WASP-*-Darwin.sh ../
cp waspConfig_install.cmake ../
