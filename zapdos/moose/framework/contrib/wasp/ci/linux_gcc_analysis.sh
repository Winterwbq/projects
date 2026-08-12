if [[ -d build  ]]; then
   rm -rf build
fi

mkdir -p build/miniconda3
cd build

# Setup a conda install
wget https://code-int.ornl.gov/lefebvre/miniconda/-/raw/main/Miniconda3-py310_24.5.0-0-Linux-x86_64.sh -O ${PWD}/miniconda3/miniconda.sh
bash ${PWD}/miniconda.sh -b -u -p ${PWD}/miniconda3
eval "$(${PWD}/miniconda3/bin/conda shell.bash hook 2> /dev/null)"
export CONDA_NUMBER_CHANNEL_NOTICES=0
conda env create -f ../ci/env.yml
conda activate wasp_ci
conda install -y  conda-forge::valgrind -c conda-forge --override-channels

cmake -DBUILDNAME="$(uname -s)-Analysis-Debug-${CI_COMMIT_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=DEBUG \
      -DPython3_EXECUTABLE=`which python`\
      -Dwasp_ENABLE_COVERAGE_TESTING=ON \
      -DCOVERAGE_EXTRA_FLAGS="-s ${CI_PROJECT_DIR}/googletest -d" \
      -Dwasp_ENABLE_TESTS=ON \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      -DCMAKE_CXX_COMPILER:FILEPATH=`which g++` \
      -DCMAKE_C_COMPILER:FILEPATH=`which gcc` \
      -S ../ -B ./

export CMAKE_BUILD_PARALLEL_LEVEL=8

ctest --output-on-failure \
      -D Experimental -j 8 \
      -D ExperimentalMemCheck \
      -D ExperimentalCoverage 

