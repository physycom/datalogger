#!/usr/bin/env pwsh

mkdir -Force build
cd build

cmake .. -G "Visual Studio 15" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=$env:VCPKG_DEFAULT_TRIPLET" "-DCMAKE_BUILD_TYPE=Release"
cmake --build . --config Release

cd ..
