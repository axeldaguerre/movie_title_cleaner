@echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
SET CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DINTERNAL=1 -DINTERNAL=1 -FC -Z7

REM call cl %CommonCompilerFlags% ..\code\web_formater.cpp /link %CommonLinkerFlags% 
REM call clang -g -fuse-ld=lld ..\sim86.cpp -o sim86_clang_debug.exe

REM call cl -nologo -Zi -FC ..\website_formater.cpp -Fesim86_msvc_debug.exe
REM call clang -g -fuse-ld=lld ..\code\folder_creator.cpp -o folder_creator_debug.exe 
cl %CommonCompilerFlags% ../code/folder_creator.cpp /link -incremental:no -opt:ref user32.lib gdi32.lib Xinput.lib dsound.lib Winmm.lib wininet.lib
popd