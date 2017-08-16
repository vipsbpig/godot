#!/bin/sh

scons p=iphone target=release bits=32 tools=no arch=arm -j 3
scons p=iphone target=release bits=64 tools=no arch=arm64 -j 3
