@echo off
SETLOCAL
 
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
 
IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
 
goto end
 
:end

call godot_build_android.bat all

call godot_build_windows.bat release_debug
call godot_build_windows.bat release

call godot_build_windows_tools.bat release_debug

ENDLOCAL
echo.