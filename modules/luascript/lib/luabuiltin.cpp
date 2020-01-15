#include "luabuiltin.h"
#include "../debug.h"
#include "luabinding_helper.h"

void LuaBuiltin::regitser_builtins(lua_State *L) {
	LuaBindingHelper::stackDump(L);
	const char *buildIns[] = {
		"Vector2", //0
		"Rect2", //1
		"Color", //2
		"Vector3", //3
		"Basis", //4
		"Quat", //5
		"RID", //6
		"Transform2D", //7
		"Plane", //8
		"AABB", //9
		"Transform", //10
		"PoolByteArray", //11
		"PoolIntArray", //12
		"PoolRealArray", //13
		"PoolStringArray", //14
		"PoolVector2Array", //15
		"PoolVector3Array", //16
		"PoolColorArray", //17
	};

	lua_getfield(L, LUA_GLOBALSINDEX, "GD");

	int len = sizeof(buildIns) / sizeof(char *);
	for (int i = 0; i < len; i++) {
		lua_newtable(L);
		char dst[20];
		sprintf(dst, ".%s", buildIns[i]);
		luaL_newmetatable(L, dst);
		{
			lua_pushstring(L, "__call");
			lua_pushstring(L, buildIns[i]);
			lua_pushinteger(L, i);
			lua_pushcclosure(L, LuaBuiltin::meta_bultins__call, 2);
			lua_rawset(L, -3);
		}
		lua_setmetatable(L, -2);
		lua_setfield(L, -2, buildIns[i]);
	}

	lua_pop(L, 1);
}

int LuaBuiltin::meta_bultins__call(lua_State *L) {
	const char *builtInType = luaL_checkstring(L, lua_upvalueindex(1));
	int builtIn = lua_tointeger(L, lua_upvalueindex(2));
	int args_c = lua_gettop(L);
	print_format("%d type:%s", builtIn, builtInType);

	if (0 == builtIn) { //Vector2
		Vector2 *ptr;
		switch (args_c) {
			case 0:
				ptr = new Vector2();
				break;
			case 1: {
				Variant arg1;
				l_get_variant(L, 1, arg1);
				ptr = new Vector2(arg1);
				break;
			}
			case 2: {
				Variant arg1;
				Variant arg2;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				ptr = new Vector2(arg1, arg2);
				break;
			}
			default: return 0; break;
		}
		l_push_bulltins_type(L, *ptr);
		delete ptr;
		return 1;

	} else if (3 == builtIn) { //Vector3
		Vector3 *ptr;
		switch (args_c) {
			case 0:
				ptr = new Vector3();
				break;
			case 1: {
				Variant arg1;
				l_get_variant(L, 1, arg1);
				ptr = new Vector3(arg1);
				break;
			}
			case 3: {
				Variant arg1;
				Variant arg2;
				Variant arg3;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				l_get_variant(L, 3, arg3);
				ptr = new Vector3(arg1, arg2, arg3);
				break;
			}
			default: return 0; break;
		}
		l_push_bulltins_type(L, *ptr);
		delete ptr;
		return 1;

	} else if (4 == builtIn) { //Basis
		Basis *ptr;
		switch (args_c) {
			case 0:
				ptr = new Basis();
				break;
			case 1: {
				Variant arg1;
				l_get_variant(L, 1, arg1);
				ptr = new Basis(arg1);
				break;
			}
			case 2: {
				Variant arg1;
				Variant arg2;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				if (arg2.get_type() == Variant::VECTOR3)
					ptr = new Basis((Vector3)arg1, (Vector3)arg2);
				else
					ptr = new Basis(arg1, (real_t)arg2);
				break;
			}
			case 3: {
				Variant arg1;
				Variant arg2;
				Variant arg3;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				l_get_variant(L, 3, arg3);
				if (arg2.get_type() == Variant::VECTOR3)
					ptr = new Basis(arg1, (Vector3)arg2, arg3);
				else
					ptr = new Basis(arg1, (real_t)arg2, arg3);
				break;
			}
			default: return 0; break;
		}
		l_push_bulltins_type(L, *ptr);
		delete ptr;
		return 1;

	} else if (7 == builtIn) { //Transform2D
		Transform2D *ptr;
		switch (args_c) {
			case 0:
				ptr = new Transform2D();
				break;
			case 1: {
				Variant arg1;
				l_get_variant(L, 1, arg1);
				ptr = new Transform2D(arg1);
				break;
			}
			case 2: {
				Variant arg1;
				Variant arg2;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				ptr = new Transform2D(arg1, arg2);
				break;
			}
			default: return 0; break;
		}
		l_push_bulltins_type(L, *ptr);
		return 1;
		delete ptr;

	} else if (10 == builtIn) { //Transform
		Transform *ptr;
		switch (args_c) {
			case 0:
				ptr = new Transform();
				break;
			case 1: {
				Variant arg1;
				l_get_variant(L, 1, arg1);
				ptr = new Transform(arg1);
				break;
			}
			case 2: {
				Variant arg1;
				Variant arg2;
				l_get_variant(L, 1, arg1);
				l_get_variant(L, 2, arg2);
				ptr = new Transform(arg1, arg2);
				break;
			}
			default: return 0; break;
		}
		l_push_bulltins_type(L, *ptr);
		return 1;
		delete ptr;

	} else {
		luaL_error(L, "TODO builtIn type:%s", builtInType);
	}
	return 0;
}