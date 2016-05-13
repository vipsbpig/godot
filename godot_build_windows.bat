@echo off
SETLOCAL
 
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
 
IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
 
goto end
 
:build_DEBUG
echo No Need To Build Debug !

goto end
 
:build_RELEASE_DEBUG
echo Building Release_Debug 32bits!
call scons platform=windows target=release_debug bits=32 tools=no -j 7
copy ".\bin\godot.windows.opt.debug.32.exe" ".\templates\windows_32_debug.exe"
call upx -9 ".\templates\windows_32_debug.exe"
echo Building Release_Debug 64bits!
call scons platform=windows target=release_debug bits=64 tools=no -j 7
copy ".\bin\godot.windows.opt.debug.64.exe" ".\templates\windows_64_debug.exe"
call upx -9 ".\templates\windows_64_debug.exe"
goto end
 
:build_RELEASE
echo Building Release 32bits!
call scons platform=windows target=release bits=32 tools=no -j 7
copy ".\bin\godot.windows.opt.32.exe" ".\templates\windows_32_release.exe"
call upx -9 ".\templates\windows_32_release.exe"
echo Building Release 64bits!
call scons platform=windows target=release bits=64 tools=no -j 7
copy ".\bin\godot.windows.opt.64.exe" ".\templates\windows_64_release.exe"
call upx -9 ".\templates\windows_64_release.exe"
goto end
 
:end
ENDLOCAL
echo.