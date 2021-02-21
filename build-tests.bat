@echo off

set prjName=Hyped
set target=Tests
set files=%cd%\tests\test.cpp
set binDir=%cd%\bin\Win-x64-%target%
set binIntDir=%cd%\bin-int\Win-x64-%target%

if not exist %binDir% mkdir %binDir%
if not exist %binIntDir% mkdir %binIntDir%
if not exist %binIntDir%\resources.res (
	call rc -nologo -r -fo %binIntDir%\resources.res %cd%\resources\resources.rc
)

set sharedCompilerFlags=^
	-Oi -MD -O2 -GR- -EHsc -FC -nologo -std:c++17 ^
	-W4 -wd4201 -wd4100 -wd4189 -wd4505 ^
	-Fe:%binDir%\%prjName% ^
	-Fo:%binIntDir%\%prjName% ^
	-I %cd%\src ^
	-I %cd%\deps ^
	-I %cd%\resources ^
	resources.res


set  sharedLinkerFlags=-WX -opt:ref -incremental:no -subsystem:console -LIBPATH:%binIntDir%
set  sharedLibs=kernel32.lib user32.lib

call .\ctime\ctime -begin %prjName%_Win-x64-%target%.ctime

cl %sharedCompilerFlags% %files% /link %sharedLinkerFlags% %sharedLibs%

call .\ctime\ctime -end %prjName%_Win-x64-%target%.ctime %ERRORLEVEL%

echo Program terminated with code %ERRORLEVEL%.
EXIT /B %ERRORLEVEL%
