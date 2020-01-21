#ifndef LUAHEADER_H
#define LUAHEADER_H

extern "C" {
#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM == 501
#include "../cmodule/compat53/lprefix.h"
#else
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#endif
}

#endif // LUAHEADER_H
