#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

echo "Installing library dependencies..."
packages="$(tr '\n' ' ' <<< "$(cat "${DIR}/vcpkg-packages.txt")")"
if [ ! -z "${packages}" -a "${packages}" != " " ]; then
    "${DIR}/tools/vcpkg/vcpkg" install ${packages}
fi
