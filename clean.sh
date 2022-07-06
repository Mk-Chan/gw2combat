#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

echo "Deleting build directory..."
rm -rf "${DIR}/build"

echo "Done!"
