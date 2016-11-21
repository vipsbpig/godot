@echo off
SETLOCAL

IF "%1" == "debug" goto build_DEBUG
IF "%1" == "release" goto build_RELEASE
IF "%1" == "release_debug" goto build_RELEASE_DEBUG
IF "%1" == "all" goto build_ALL

goto end

:build_DEBUG
echo No Need To Build Debug!

goto end


:build_RELEASE_DEBUG
echo Building Release_Debug!
call scons platform=android target=release_debug tools=no -j 7
cd platform/android/java/
call gradlew.bat build --info > bugtags.log
cd ..\..\..
copy "bin\android_debug.apk"  "%USERPROFILE%\AppData\Roaming\Godot\templates\android_debug.apk"
goto end

:build_RELEASE
echo Building Release!
call scons platform=android target=release tools=no -j 7
cd platform/android/java/
call gradlew.bat build --info > bugtags.log
cd ..\..\..
copy "bin\android_release.apk"  "%USERPROFILE%\AppData\Roaming\Godot\templates\android_release.apk"
goto end

:build_ALL
echo Building Release_Debug!
call scons platform=android target=release_debug tools=no -j 7
echo Building Release!
call scons platform=android target=release tools=no -j 7
cd platform/android/java/
call gradlew.bat build
cd ..\..\..
copy "bin\android_debug.apk"  "%USERPROFILE%\AppData\Roaming\Godot\templates\android_debug.apk"
copy "bin\android_debug.apk"  "templates"
copy "bin\android_release.apk"  "%USERPROFILE%\AppData\Roaming\Godot\templates\android_release.apk"
copy "bin\android_release.apk"  "templates"

:end
ENDLOCAL
echo.
