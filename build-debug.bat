@echo off

set prjName=Hyped
set target=Debug
set files=%cd%\src\win32_hyper.c
set binDir=%cd%\bin\Win-x64-%target%
set binIntDir=%cd%\bin-int\Win-x64-%target%

if not exist %binDir% mkdir %binDir%
if not exist %binIntDir% mkdir %binIntDir%
if not exist %binIntDir%\resources.res (
    call rc -nologo -r -fo %binIntDir%\resources.res %cd%\resources\resources.rc
)

set sharedCompilerFlags=^
    -Oi -MTd -Zi -GR- -EHa- -FC -nologo -std:c++17 ^
    -W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 ^
    -Fe:%binDir%\%prjName% ^
    -Fo:%binIntDir%\%prjName% ^
    -Fm:%binDir%\%prjName% ^
    -Fd:%binDir%\%prjName% ^
    -I %cd%\src ^
    -I %cd%\deps ^
    -I %cd%\resources ^
    -D HY_SLOW ^
    -D HY_ENABLE_LOG ^
    resources.res

set  sharedLinkerFlags=-WX -opt:ref -incremental:no -subsystem:console -LIBPATH:%binIntDir%
set  sharedLibs=kernel32.lib user32.lib gdi32.lib opengl32.lib dwmapi.lib shell32.lib

call .\ctime\ctime -begin %prjName%_Win-x64-%target%.ctime

cl %sharedCompilerFlags% %files% /link %sharedLinkerFlags% %sharedLibs%

call .\ctime\ctime -end %prjName%_Win-x64-%target%.ctime %ERRORLEVEL%

echo Program terminated with code %ERRORLEVEL%.
EXIT /B %ERRORLEVEL%

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
