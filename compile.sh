#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

./dependencies-install.sh

TARGET=${1:-all}
BUILD_TYPE=${2:-Debug}
BUILD_TYPE_LOWERCASE="$(tr '[:upper:]' '[:lower:]' <<< "${BUILD_TYPE}")"
CMAKE_BUILD_TYPE="$(tr '[:lower:]' '[:upper:]' <<< "${BUILD_TYPE_LOWERCASE:0:1}")${BUILD_TYPE_LOWERCASE:1}"

BUILD_THREADS=${3:-1}

CMAKE_BUILD_DIR=build/${BUILD_TYPE_LOWERCASE}
mkdir -p "${CMAKE_BUILD_DIR}"
cd "${CMAKE_BUILD_DIR}"

echo "Running CMake..."
cmake -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
    -DCMAKE_TOOLCHAIN_FILE="${DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake" \
    ../..

echo "Building..."
cmake --build . --target ${TARGET} -- -j ${BUILD_THREADS}

echo "Done!"
