#ifndef LUABUILTIN_H
#define LUABUILTIN_H

#include "core/object.h"
#include "lua.hpp"

class LuaBuiltin {
	typedef struct {
		const char *c_char;
		const StringName *p_sn;
	} Char_Psn;
	static Char_Psn quickSearch[];

public:
	//====
	static const char GD_ARRAY;
	
	static const StringName *GetVariantPropStringName(const char *p_input, bool &founded);
	static void regitser_builtins(lua_State *L);
	static int meta_bultins__call(lua_State *L);
};
#endif // LUABUILTIN_H
