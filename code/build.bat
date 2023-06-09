@echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DINTERNAL=1  -DWIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib Wininet.lib Wininet.lib
cl %CommonCompilerFlags% ..\code\purificator.cpp -Fmpurificator.map -LD /link -incremental:no -opt:ref -PDB:purificator.pdb -EXPORT:GetMovieData -EXPORT:CleanFileMovie -EXPORT:AskPickMovies -EXPORT:DEBUGCheckUserPicksFile

cl %CommonCompilerFlags% ..\code\win32_purificator.cpp -Fmwin32_purificator.map /link %CommonLinkerFlags%

popd
