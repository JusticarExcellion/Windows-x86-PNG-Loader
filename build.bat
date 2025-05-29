@echo off

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4456 -wd4505 -DDEBUG_PNG=1 -DDEBUG_SLOW=1 -DBUILD_PNG=0 -Z7 -FC
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib Gdi32.lib winmm.lib ole32.lib

mkdir G:\Build
pushd  G:\Build

cl %CommonCompilerFlags%  G:\ProjectFiles\PNGLoader.cpp /link %CommonLinkerFlags%

popd
