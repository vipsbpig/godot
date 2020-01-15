#include "luabuiltin.h"
#include "../debug.h"
#include "luabinding_helper.h"

void LuaBuiltin::regitser_builtins(lua_State *L) {
	LuaBindingHelper::stackDump(L);
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
}

int LuaBuiltin::meta_bultins__call(lua_State *L) {
	Variant::Type type = (Variant::Type)lua_tointeger(L, lua_upvalueindex(1));
	const char *type_name = lua_tostring(L, lua_upvalueindex(2));
	int top = lua_gettop(L);
	Variant::CallError err;
	Variant ret;
	if (top <= 1) {

		ret = Variant::construct(type, NULL, 0, err);
	} else {

		Variant *vars = memnew_arr(Variant, top - 1);
		Variant *args[128];
		for (int idx = 2; idx <= top; idx++) {

			Variant &var = vars[idx - 2];
			args[idx - 2] = &var;
			l_get_variant(L, idx, var);
		}
		ret = Variant::construct(type, (const Variant **)(args), top - 1, err);
		memdelete_arr(vars);
	}
	switch (err.error) {
		case Variant::CallError::CALL_OK:
			l_push_variant(L, ret);
			return 1;
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