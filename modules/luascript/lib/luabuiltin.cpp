#include "luabuiltin.h"
#include "../debug.h"
#include "core/core_string_names.h"
#include "luabinding_helper.h"

void LuaBuiltin::regitser_builtins(lua_State *L) {
	typedef struct {
		const char *type;
		Variant::Type vt;
	} BulitinTypes;
	const BulitinTypes buildIns[] = {
		{ "Vector2", Variant::VECTOR2 }, //0
		{ "Rect2", Variant::RECT2 }, //1
		{ "Color", Variant::COLOR }, //2
		{ "Vector3", Variant::VECTOR3 }, //3
		{ "Basis", Variant::BASIS }, //4
		{ "Quat", Variant::QUAT }, //5
		{ "RID", Variant::_RID }, //6
		{ "Transform2D", Variant::TRANSFORM2D }, //7
		{ "Plane", Variant::PLANE }, //8
		{ "AABB", Variant::AABB }, //9
		{ "Transform", Variant::TRANSFORM }, //10
		{ "PoolByteArray", Variant::POOL_BYTE_ARRAY }, //11
		{ "PoolIntArray", Variant::POOL_INT_ARRAY }, //12
		{ "PoolRealArray", Variant::POOL_REAL_ARRAY }, //13
		{ "PoolStringArray", Variant::POOL_STRING_ARRAY }, //14
		{ "PoolVector2Array", Variant::POOL_VECTOR2_ARRAY }, //15
		{ "PoolVector3Array", Variant::POOL_VECTOR3_ARRAY }, //16
		{ "PoolColorArray", Variant::POOL_COLOR_ARRAY }, //17
	};

	lua_getfield(L, LUA_GLOBALSINDEX, "GD");
	int len = sizeof(buildIns) / sizeof(BulitinTypes);
	for (int i = 0; i < len; i++) {
		lua_newtable(L);
		char dst[20];
		snprintf(dst, 20, ".%s", buildIns[i].type);
		luaL_newmetatable(L, dst);
		{
			lua_pushstring(L, "__call");
			lua_pushinteger(L, buildIns[i].vt);
			lua_pushstring(L, buildIns[i].type);
			lua_pushcclosure(L, LuaBuiltin::meta_bultins__call, 2);
			lua_rawset(L, -3);
		}
		lua_setmetatable(L, -2);
		lua_setfield(L, -2, buildIns[i].type);
	}
	lua_pop(L, 1);

	typedef struct {
		const char *c_char;
		const StringName *p_sn;
	} Char_Psn;
	const Char_Psn quickSearch[] = {
		{ "x", &CoreStringNames::get_singleton()->x }, //0
		{ "y", &CoreStringNames::get_singleton()->y }, //0
		{ "z", &CoreStringNames::get_singleton()->z }, //0
		{ "w", &CoreStringNames::get_singleton()->w }, //0
		{ "r", &CoreStringNames::get_singleton()->r }, //0
		{ "g", &CoreStringNames::get_singleton()->g }, //0
		{ "b", &CoreStringNames::get_singleton()->b }, //0
		{ "a", &CoreStringNames::get_singleton()->a }, //0
		{ "position", &CoreStringNames::get_singleton()->position }, //0
		{ "size", &CoreStringNames::get_singleton()->size }, //0
		{ "end", &CoreStringNames::get_singleton()->end }, //0
		{ "basis", &CoreStringNames::get_singleton()->basis }, //0
		{ "origin", &CoreStringNames::get_singleton()->origin }, //0
		{ "normal", &CoreStringNames::get_singleton()->normal }, //0
		{ "d", &CoreStringNames::get_singleton()->d }, //0
		{ "h", &CoreStringNames::get_singleton()->h }, //0
		{ "s", &CoreStringNames::get_singleton()->s }, //0
		{ "v", &CoreStringNames::get_singleton()->v }, //0
		{ "r8", &CoreStringNames::get_singleton()->r8 }, //0
		{ "g8", &CoreStringNames::get_singleton()->g8 }, //0
		{ "b8", &CoreStringNames::get_singleton()->b8 }, //0
		{ "a8", &CoreStringNames::get_singleton()->a8 }, //0
	};
	//Vaiant property to string
	lua_newtable(L);
	len = sizeof(quickSearch) / sizeof(Char_Psn);
	for (int i = 0; i < len; i++) {
		lua_pushlightuserdata(L, (void *)quickSearch[i].p_sn);
		lua_setfield(L, -2, quickSearch[i].c_char);
	}
	lua_setfield(L, LUA_REGISTRYINDEX, "VariantProps");
}

int LuaBuiltin::meta_bultins__call(lua_State *L) {
	Variant::Type type = (Variant::Type)lua_tointeger(L, lua_upvalueindex(1));
	const char *type_name = lua_tostring(L, lua_upvalueindex(2));
	int top = lua_gettop(L);
	Variant::CallError err;
	if (top <= 1) {

		Variant &&ret = Variant::construct(type, NULL, 0, err);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	} else {

		Variant *vars = memnew_arr(Variant, top - 1);
		Variant *args[128];
		for (int idx = 2; idx <= top; idx++) {

			Variant &var = vars[idx - 2];
			args[idx - 2] = &var;
			l_get_variant(L, idx, var);
		}
		Variant &&ret = Variant::construct(type, (const Variant **)(args), top - 1, err);
		memdelete_arr(vars);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	}
	switch (err.error) {
		case Variant::CallError::CALL_ERROR_INVALID_METHOD:
			luaL_error(L, "Invalid method");
			break;
		case Variant::CallError::CALL_ERROR_INVALID_ARGUMENT:
			luaL_error(L, "Invalid argument to construct built-in type '%s", type_name);
			break;
		case Variant::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
			luaL_error(L, "Too many arguments to construct built-in type '%s", type_name);
			break;
		case Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
			luaL_error(L, "Too few arguments to construct built-in type '%s", type_name);
			break;
		case Variant::CallError::CALL_ERROR_INSTANCE_IS_NULL:
			luaL_error(L, "Instance is null");
			break;
	}
	return 0;
}