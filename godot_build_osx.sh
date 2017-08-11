#!/bin/sh

scons p=osx target=release bits=32 tools=no -j 3
#scons p=osx target=release_debug bits=32 tools=no -j 3
#scons p=osx target=release bits=64 tools=no -j 3
#scons p=osx target=release_debug bits=64 tools=no -j 3


cp -r misc/dist/osx_template.app/ bin/
mkdir -p bin/osx_template.app/Contents/MacOS
cp bin/godot.osx.opt.32 bin/osx_template.app/Contents/MacOS/
#cp bin/godot.osx.opt.debug.32 bin/osx_template.app/Contents/MacOS/
#cp bin/godot.osx.opt.64 bin/osx_template.app/Contents/MacOS/
#cp bin/godot.osx.opt.debug.64 bin/osx_template.app/Contents/MacOS/

zip -r9 bin/osx.zip bin/osx_template.app
