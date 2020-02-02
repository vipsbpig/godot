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
	static const char GD_VECTOR2;
	static const char GD_RECT2;
	static const char GD_VECTOR3;
	static const char GD_TRANSFORM2D;
	static const char GD_PLANE;
	static const char GD_QUAT;
	static const char GD_AABB;
	static const char GD_BASIS;
	static const char GD_TRANSFORM;
	static const char GD_COLOR;
	static const char GD_DICTIONARY;
	static const char GD_ARRAY;

	static const StringName *GetVariantPropStringName(const char *p_input, bool &founded);
	static void regitser_builtins(lua_State *L);
	static int meta_bultins__call(lua_State *L);

	//built in metatable

	static int meta_builtins__evaluate(lua_State *L);
	static int meta_builtins__tostring(lua_State *L);
	static int meta_builtins__index(lua_State *L);
	static int l_variants_caller_wrapper(lua_State *L);

	//===builtin
	static Vector2 l_get_vector2(lua_State *L, int idx);
	static Rect2 l_get_rect2(lua_State *L, int idx);
	static Vector3 l_get_vector3(lua_State *L, int idx);
	static Transform2D l_get_transform2d(lua_State *L, int idx);
	static Plane l_get_Plane(lua_State *L, int idx);
	static Quat l_get_quat(lua_State *L, int idx);
	static AABB l_get_aabb(lua_State *L, int idx);
	static Basis l_get_basis(lua_State *L, int idx);
	static Transform l_get_transform(lua_State *L, int idx);
	static Color l_get_color(lua_State *L, int idx);
	static Dictionary l_get_dict(lua_State *L, int idx);
	static Array l_get_array(lua_State *L, int idx);

	static void l_push_vector2_type(lua_State *L, const Vector2 &var);
	static void l_push_rect2_type(lua_State *L, const Rect2 &var);
	static void l_push_vector3_type(lua_State *L, const Vector3 &var);
	static void l_push_transform2d_type(lua_State *L, const Transform2D &var);
	static void l_push_plane_type(lua_State *L, const Plane &var);
	static void l_push_quat_type(lua_State *L, const Quat &var);
	static void l_push_aabb_type(lua_State *L, const AABB &var);
	static void l_push_basis_type(lua_State *L, const Basis &var);
	static void l_push_transform_type(lua_State *L, const Transform &var);
	static void l_push_color_type(lua_State *L, const Color &var);
	static void l_push_dict_type(lua_State *L, const Dictionary &var);
	static void l_push_array_type(lua_State *L, const Array &var);
};
#endif // LUABUILTIN_H
