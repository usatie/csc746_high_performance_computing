#!/bin/bash

#rsync -avz --exclude='build' --exclude='.git' --exclude='.gitignore' --exclude='remote-build.sh' --exclude='README.md' . perlmutter.nersc.gov:csc746-cp4-mmul-omp/
# rsync only .cpp files
rsync -avz --include='*.cpp' --include='*.sh' --include='*.h' --exclude='*' . perlmutter.nersc.gov:csc746-cp4-mmul-omp/

# This script will build the project on NERSC
sshnersc 'bash -s' << EOF
cd csc746-cp4-mmul-omp/

# create a build directory and run cmake
if [! -d "build"]; then
	mkdir -p build
	cd build
	cmake .. -Wno-dev
else
	cd build
fi

# build the project
make
ls | grep job | xargs chmod +x
EOF
