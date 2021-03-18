@echo off

set prjName=Hyped
set target=Debug
set files=%cd%\src\win32_main.c
set buildDir=%cd%\build\Win-x64-%target%

call .\3rd\ctime\ctime -begin %prjName%_Win-x64-%target%.ctime

set compilerFlags=^
	-diagnostics:column ^
    -Oi -MTd -Zi -GR- -EHa- -FC -nologo -std:c++17 ^
    -W4 -WX -wd4201 -wd4100 -wd4189 ^
    -I %cd%\src ^
    -I %cd%\3rd ^
    -I %cd%\3rd\cglm\include ^
    -I %cd%\3rd\libgit2\include ^
    -D HY_SLOW ^
    -D HY_ENABLE_LOG

set linkerFlags=-WX -opt:ref -incremental:no -subsystem:console
set libs=kernel32.lib user32.lib gdi32.lib opengl32.lib dwmapi.lib shell32.lib uxtheme.lib git2.lib -LIBPATH:%cd%\3rd\libgit2\debug\lib

if not exist %buildDir% mkdir %buildDir%
if not exist %buildDir%\git2.dll copy %cd%\3rd\libgit2\debug\bin\git2.dll %buildDir%
if not exist %buildDir%\zlibd1.dll copy %cd%\3rd\libgit2\debug\bin\zlibd1.dll %buildDir%
if not exist %buildDir%\pcre.dll copy %cd%\3rd\libgit2\release\bin\pcre.dll %buildDir%
if not exist %buildDir%\assets mklink /D %buildDir%\assets %cd%\assets
pushd %buildDir%
	del *.pdb > NUL 2> NUL
	cl %compilerFlags% %files% /link %linkerFlags% %libs% /out:%prjName%.exe
popd

call .\3rd\ctime\ctime -end %prjName%_Win-x64-%target%.ctime %ERRORLEVEL%

:: ╔════════════════╗
:: ║ Compiler Flags ║
:: ╚════════════════╝

:: Optimization
:: Oi            Generates intrinsic functions.
:: O2            Creates fast code.
:: GR-           Disables run-time type information (RTTI).
:: EHa-          Disables exception handling.
:: Fd:<file>     Renames program database file.
:: Fe:<file.exe> Renames the executable file.
:: Fo:<file>     Creates an object file.
:: Fm:<file>     Creates a mapfile.
:: D <name>      Defines constants and macros.
:: I <dir>       Searches a directory for include files.
:: Zi            Generates complete debugging information.
:: LD            Creates a dynamic-link library.
:: MD            Compiles to create a multithreaded DLL, by using MSVCRT.lib.
:: FC            Display full path of source code files passed to cl.exe in diagnostic text.
:: nologo        Suppresses display of sign-on banner
:: WX            Treats all warnings as errors.
:: W4            Sets output warning level.
:: wd<nnn>       Disables the specified warning.

:: ╔═══════════════════╗
:: ║ Compiler Warnings ║
:: ╚═══════════════════╝

:: 4201 nonstandard extension used : nameless struct/union
:: 4100 'identifier' : unreferenced formal parameter
:: 4189 'identifier' : local variable is initialized but not referenced (will be optimized away by the compiler).

:: ╔══════════════╗
:: ║ Linker Flags ║
:: ╚══════════════╝

:: incremental:no Controls incremental linking (disabled).
:: opt:ref        Controls LINK optimizations (remove non referenced functions and data).
:: subsystem      Tells the operating system how to run the .exe file.
:: LIBPATH:<dir>  Specifies a path to search before the environmental library path.
