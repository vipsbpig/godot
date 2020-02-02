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
	static const char GD_VECTOR2;
	static const char GD_RECT2;

	static const StringName *GetVariantPropStringName(const char *p_input, bool &founded);
	static void regitser_builtins(lua_State *L);
	static int meta_bultins__call(lua_State *L);

	//===builtin

	static Vector2 l_get_vector2(lua_State *L, int idx);
	static Rect2 l_get_rect2(lua_State *L, int idx);
	static Array l_get_array(lua_State *L, int idx);

	static void l_push_vector2_type(lua_State *L, const Vector2 &var);
	static void l_push_rect2_type(lua_State *L, const Rect2 &var);
	static void l_push_array_type(lua_State *L, const Variant &var);
	
};
#endif // LUABUILTIN_H
