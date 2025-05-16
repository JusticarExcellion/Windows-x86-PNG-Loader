@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
rem Put the path directory for windows to look into before the path with the [DirectoryName];%path%
set path = w:\ConwayGameOfLife;%path%
