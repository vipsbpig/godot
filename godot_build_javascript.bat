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
call scons platform=javascript target=release_debug tools=no module_openssl_enabled=no -j 7

:zip_RELEASE_DEBUG
mkdir templates\html_fs
copy tools\dist\html_fs templates\html_fs
copy bin\godot.javascript.opt.debug.exe.html.mem templates\html_fs
copy bin\godot.javascript.opt.debug.exe.js templates\html_fs
cd templates\html_fs
rename godot.javascript.opt.debug.exe.html.mem godot.mem
rename godot.javascript.opt.debug.exe.js godot.js
7z a ..\javascript_debug.zip godot.html godot.js godot.mem godotfs.js
cd ..\..
rmdir /s/q templates\html_fs
goto end

:build_RELEASE
echo Building Release !
call scons platform=javascript target=release tools=no module_openssl_enabled=no -j 7
echo Building Release 64bits!

:zip_RELEASE
mkdir templates\html_fs
copy tools\dist\html_fs\godotfs.js templates\html_fs
copy bin\godot.javascript.opt.exe.html templates\html_fs
copy bin\godot.javascript.opt.exe.html.mem templates\html_fs
copy bin\godot.javascript.opt.exe.js templates\html_fs
cd templates\html_fs
rename godot.javascript.opt.exe.html godot.html
rename godot.javascript.opt.exe.html.mem godot.mem
rename godot.javascript.opt.exe.js godot.js
7z a ..\javascript_release.zip godot.html godot.js godot.mem godotfs.js
cd ..\..
rmdir /s/q templates\html_fs

goto end

:build_ALL
echo Building Release_Debug !
call scons platform=javascript target=release_debug tools=no module_openssl_enabled=no -j 7

echo Building Release !
call scons platform=javascript target=release tools=no module_openssl_enabled=no -j 7


mkdir templates\html_fs
copy tools\dist\html_fs templates\html_fs
copy bin\godot.javascript.opt.debug.exe.html.mem templates\html_fs
copy bin\godot.javascript.opt.debug.exe.js templates\html_fs
cd templates\html_fs
rename godot.javascript.opt.debug.exe.html.mem godot.mem
rename godot.javascript.opt.debug.exe.js godot.js
7z a ..\javascript_debug.zip godot.html godot.js godot.mem godotfs.js
cd ..\..
rmdir /s/q templates\html_fs

mkdir templates\html_fs
copy tools\dist\html_fs\godotfs.js templates\html_fs
copy bin\godot.javascript.opt.exe.html templates\html_fs
copy bin\godot.javascript.opt.exe.html.mem templates\html_fs
copy bin\godot.javascript.opt.exe.js templates\html_fs
cd templates\html_fs
rename godot.javascript.opt.exe.html godot.html
rename godot.javascript.opt.exe.html.mem godot.mem
rename godot.javascript.opt.exe.js godot.js
7z a ..\javascript_release.zip godot.html godot.js godot.mem godotfs.js
cd ..\..
rmdir /s/q templates\html_fs

:end
ENDLOCAL
echo.
