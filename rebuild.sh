#!/bin/bash
set -e

BUILD_TYPE=${1:-Debug}

echo "=== Build : ${BUILD_TYPE} ==="

rm -rf build

cmake -S . \
      -B build \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

# cmake --build build -j"$(nproc)" -- VERBOSE=1
cmake --build build -j"$(nproc)"