#!/bin/bash

sshnersc 'bash -s' <<'EOF'
cd csc746-cp4-mmul-omp/
./test.sh
EOF
