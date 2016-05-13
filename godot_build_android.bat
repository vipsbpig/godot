@echo off
SETLOCAL
 
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
 
IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
 
goto end
 
:build_DEBUG
echo No Need To Build Debug!

goto end
 
:build_RELEASE_DEBUG
echo Building Release_Debug!
call scons platform=android target=release_debug tools=no -j 7
copy ".\bin\libgodot.android.opt.debug.armv7.neon.so" ".\platform\android\java\libs\armeabi\libgodot_android.so"
cd platform/android/java/
call gradlew.bat build
copy ".\build\outputs\apk\java-release-unsigned.apk" "..\..\..\templates\android_debug.apk"
cd ..\..\..
goto end
 
:build_RELEASE
echo Building Release!
call scons platform=android target=release tools=no -j 7
copy ".\bin\libgodot.android.opt.armv7.neon.so" ".\platform\android\java\libs\armeabi\libgodot_android.so"
cd platform/android/java/
call gradlew.bat build
copy ".\build\outputs\apk\java-release-unsigned.apk" "..\..\..\templates\android_release.apk"
cd ..\..\..
 
:end
ENDLOCAL
echo.