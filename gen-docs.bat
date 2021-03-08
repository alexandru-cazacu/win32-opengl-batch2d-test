@echo off

pushd docs
    rmdir .\html /s /q
    call ..\3rd\doxygen\doxygen.exe ..\Doxyfile
popd