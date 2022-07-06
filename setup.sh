#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

echo "Initializing git repository"
git init
echo "Initializing and updating submodules..."
git submodule update --init

if [ ! -f "${DIR}/tools/vcpkg/vcpkg" ]; then
    echo "Bootstrapping vcpkg..."
    git submodule add -f https://github.com/microsoft/vcpkg tools/vcpkg
    "${DIR}/tools/vcpkg/bootstrap-vcpkg.sh" -disableMetrics
    echo "Built vcpkg!"
else
    echo "Found vcpkg!"
fi

echo "Done!"
