@echo off
SETLOCAL
 
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
 
IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
 
goto end
 
:build_DEBUG
echo Building Debug 64bits!
call scons platform=windows target=debug bits=64 -j 7
copy ".\bin\godot.windows.tools.64.exe" ".\..\..\bin\godot.windows.tools.64.exe"
call upx -9 ".\..\..\bin\godot.windows.tools.64.exe"
goto end
 
:build_RELEASE_DEBUG
echo Building Release_Debug 64bits!
call scons platform=windows target=release_debug bits=64 -j 7
copy ".\bin\godot.windows.opt.tools.64.exe" ".\..\..\bin\godot.windows.opt.tools.64.exe"
call upx -9 ".\..\..\bin\godot.windows.opt.tools.64.exe"
goto end
 
:build_RELEASE
echo Tools can only be built with targets 'debug' and 'release_debug'.

goto end
 
:end
ENDLOCAL
echo.