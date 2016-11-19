@echo off
SETLOCAL


IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
IF "%1" == "all" goto build_ALL

goto end

:build_DEBUG
echo No Need To Build Debug !

goto end

:build_RELEASE_DEBUG
echo Building Release_Debug !
call scons platform=javascript target=release_debug tools=no -j 7

goto end

:build_RELEASE
echo Building Release !
call scons platform=javascript target=release tools=no -j 7
echo Building Release 64bits!

goto end

:build_ALL
echo Building Release_Debug !
call scons platform=javascript target=release_debug tools=no -j 7

echo Building Release !
call scons platform=javascript target=release tools=no -j 7

:end
ENDLOCAL
echo.
