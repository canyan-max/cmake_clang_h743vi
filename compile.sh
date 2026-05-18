#!/bin/bash
set -e

BUILD_DIR="build/Debug"

[ "$1" = "clean" ] && rm -rf build

if [ ! -f "${BUILD_DIR}/build.ninja" ]; then
    echo "Running CMake..."
    cmake --preset Debug
fi

echo "Building with Ninja..."
cmake --build "${BUILD_DIR}" -j 16
echo "Done."