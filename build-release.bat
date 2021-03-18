@echo off

set prjName=Hyped
set target=Release
set files=%cd%\src\win32_main.c
set buildDir=%cd%\build\Win-x64-%target%

call .\3rd\ctime\ctime -begin %prjName%_Win-x64-%target%.ctime

set compilerFlags=^
	-Oi -MT -O2 -GR- -EHa- -FC -nologo -std:c++17 ^
	-W4 -WX -wd4201 -wd4100 -wd4189 -wd4204 ^
	-I %cd%\src ^
	-I %cd%\3rd ^
	-I %cd%\3rd\cglm\include ^
    -I %cd%\3rd\libgit2\include ^
	-I %cd%\resources ^
	resources.res

set linkerFlags=-WX -opt:ref -incremental:no -subsystem:windows
set libs=kernel32.lib user32.lib gdi32.lib opengl32.lib dwmapi.lib shell32.lib uxtheme.lib git2.lib  -LIBPATH:%cd%\3rd\libgit2\release\lib

if not exist %buildDir% mkdir %buildDir%
if not exist %buildDir%\resources.res (call rc -nologo -r -fo %buildDir%\resources.res %cd%\src\resources.rc)
if not exist %buildDir%\git2.dll copy %cd%\3rd\libgit2\release\bin\git2.dll %buildDir%
if not exist %buildDir%\zlib1.dll copy %cd%\3rd\libgit2\release\bin\zlib1.dll %buildDir%
if not exist %buildDir%\pcre.dll copy %cd%\3rd\libgit2\release\bin\pcre.dll %buildDir%
if not exist %buildDir%\assets mklink /D %buildDir%\assets %cd%\assets
pushd %buildDir%
	del *.pdb > NUL 2> NUL
	cl %compilerFlags% %files% /link %linkerFlags% %libs% /out:%prjName%.exe
popd

call .\3rd\ctime\ctime -end %prjName%_Win-x64-%target%.ctime %ERRORLEVEL%
