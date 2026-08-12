function ecc () {
    "$@"
    if [[ $? -ne 0 ]]; then exit 1; fi
}

# Setup the basic layout
if [[ -d build  ]]; then
   rm -rf build
fi

mkdir -p build/miniconda3
cd build

# Setup a conda install
wget https://code-int.ornl.gov/lefebvre/miniconda/-/raw/main/Miniconda3-py310_24.5.0-0-Linux-x86_64.sh -O ${PWD}/miniconda3/miniconda.sh
bash ${PWD}/miniconda3/miniconda.sh -b -u -p ${PWD}/miniconda3
eval "$(${PWD}/miniconda3/bin/conda shell.bash hook 2> /dev/null)"
export CONDA_NUMBER_CHANNEL_NOTICES=0
conda env create -f ../ci/env.yml
conda activate wasp_ci

cmake -DBUILDNAME="$(uname -s)-GCC-5.4-Release-${CI_COMMIT_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dwasp_ENABLE_TESTS=ON \
      -Dwasp_ENABLE_SWIG=ON \
      -DCMAKE_C_COMPILER=/projects/gcc-5.4.0/toolset/gcc-5.4.0/bin/gcc \
      -DCMAKE_CXX_COMPILER=/projects/gcc-5.4.0/toolset/gcc-5.4.0/bin/g++ \
      -DPython3_EXECUTABLE=`which python`\
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      ..

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild -j 8\
      -D ExperimentalSubmit \
      -D ExperimentalTest -j 8 \
      -D ExperimentalSubmit
