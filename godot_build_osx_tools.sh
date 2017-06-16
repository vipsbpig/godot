#!/bin/sh

scons p=osx target=release_debug -j 3
cp -r misc/dist/osx_tools.app bin/Godot.app
mkdir -p bin/Godot.app/Contents/MacOS
cp bin/godot.osx.opt.tools.32 bin/Godot.app/Contents/MacOS/Godot
touch bin/Godot.app/Contents/MacOS/._sc_
chmod +x bin/Godot.app/Contents/MacOS/Godot
