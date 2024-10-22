#!/bin/bash

sshnersc 'bash -s' <<'EOF'
cd csc746-cp4-mmul-omp/
./bench.sh
EOF
