@echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib Wininet.lib Wininet.lib
cl %CommonCompilerFlags% ..\code\purificator.cpp -Fmpurificator.map -LD /link -incremental:no -opt:ref -PDB:purificator.pdb -EXPORT:GetMovieData

cl %CommonCompilerFlags% ..\code\win32_purificator.cpp -Fmwin32_purificator.map /link %CommonLinkerFlags%

popd




@echo off




REM TODO - can we just build both with one exe?

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM 32-bit build
REM cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp

del lock.tmp

popd
