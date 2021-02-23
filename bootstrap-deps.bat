@echo off

:: Clones vcpkg
pushd deps
    if not exist vcpkg (
        call git clone --depth 1 --branch 2020.07 https://github.com/microsoft/vcpkg
    )
popd

:: Builds vcpkg and installs required dependencies
pushd deps\vcpkg
    if not exist vcpkg.exe (
        call .\bootstrap-vcpkg.bat
    )
    call .\vcpkg install @..\..\vcpkg_x64-windows.txt
popd

:: Builds FiberTaskingLib
pushd deps\FiberTaskingLib
    if not exist build_windows (
        mkdir build_windows
    )
    call cmake -G "Visual Studio 16 2019" -A x64 -Bbuild_windows
    call cmake --build build_windows --config Debug -- /v:m /m
popd