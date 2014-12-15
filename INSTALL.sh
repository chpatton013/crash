#!/usr/bin/env bash

set -e

root_dir="$(cd "$(dirname "$0")" && pwd)"
cd "$root_dir"

# Dependency: Submodules
git submodule update --init --recursive
git submodule foreach git pull origin master

( # Dependency: Boost libraries
   cd "$root_dir/external/boost";
   ./bootstrap.sh --prefix="$root_dir";
   ./b2 -sBOOST_ROOT="$root_dir"
)

( # Dependency: GLFW library
   cd "$root_dir/external/glfw";
   cmake -Wno-dev -D BUILD_SHARED_LIBS=ON "$root_dir/external/glfw";
   make
)

# Local project source
premake4 gmake
make
