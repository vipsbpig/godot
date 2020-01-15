#ifndef LUABUILTIN_H
#define LUABUILTIN_H

#include "core/object.h"
#include "lua.hpp"

class LuaBuiltin {
public:
	static void regitser_builtins(lua_State *L);
	static int meta_bultins__call(lua_State *L);
};
#endif // LUABUILTIN_H
