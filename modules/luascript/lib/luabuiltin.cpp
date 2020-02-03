﻿#include "luabuiltin.h"
#include "core/core_string_names.h"
#include "core/variant_call.cpp"
#include "luabinding_helper.h"

LuaBuiltin::Char_Psn LuaBuiltin::quickSearch[] = {
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

void shallow_copy(lua_State *L, int index) {
	if (index < 0) {
		index = lua_gettop(L) + index + 1;
	}
	lua_newtable(L);
	lua_pushnil(L);
	while (lua_next(L, index) != 0) {
		lua_pushvalue(L, -2);
		lua_insert(L, -2);
		lua_settable(L, -4);
	}
}

const char LuaBuiltin::GD_VECTOR2 = 0;
const char LuaBuiltin::GD_RECT2 = 1;
const char LuaBuiltin::GD_VECTOR3 = 2;
const char LuaBuiltin::GD_TRANSFORM2D = 3;
const char LuaBuiltin::GD_PLANE = 4;
const char LuaBuiltin::GD_QUAT = 5;
const char LuaBuiltin::GD_AABB = 6;
const char LuaBuiltin::GD_BASIS = 7;
const char LuaBuiltin::GD_TRANSFORM = 8;
const char LuaBuiltin::GD_COLOR = 9;
const char LuaBuiltin::GD_DICTIONARY = 10;
const char LuaBuiltin::GD_ARRAY = 11;

const StringName *LuaBuiltin::GetVariantPropStringName(const char *p_input, bool &founded) {
	int len = sizeof(quickSearch) / sizeof(Char_Psn);
	for (int i = 0; i < len; i++) {
		if (strcmp(p_input, quickSearch[i].c_char) == 0) {
			founded = true;
			return quickSearch[i].p_sn;
		}
	}
	return NULL;
}

void LuaBuiltin::regitser_builtins(lua_State *L) {
	typedef struct {
		const char *type;
		Variant::Type vt;
		void *ptr;
	} BulitinTypes;
	const BulitinTypes buildIns[] = {
		{ "Vector2", Variant::VECTOR2, (void *)&LuaBuiltin::GD_VECTOR2 },
		{ "Rect2", Variant::RECT2, (void *)&LuaBuiltin::GD_RECT2 },
		{ "Vector3", Variant::VECTOR3, (void *)&LuaBuiltin::GD_VECTOR3 },
		{ "Transform2D", Variant::TRANSFORM2D, (void *)&LuaBuiltin::GD_TRANSFORM2D },
		{ "Plane", Variant::PLANE, (void *)&LuaBuiltin::GD_PLANE },
		{ "Quat", Variant::QUAT, (void *)&LuaBuiltin::GD_QUAT },
		{ "AABB", Variant::AABB, (void *)&LuaBuiltin::GD_AABB },
		{ "Basis", Variant::BASIS, (void *)&LuaBuiltin::GD_BASIS },
		{ "Transform", Variant::TRANSFORM, (void *)&LuaBuiltin::GD_TRANSFORM },
		{ "Color", Variant::COLOR, (void *)&LuaBuiltin::GD_COLOR },
		{ "RID", Variant::_RID, NULL },
		{ "Dictionary", Variant::DICTIONARY, (void *)&LuaBuiltin::GD_DICTIONARY },
		{ "Array", Variant::ARRAY, (void *)&LuaBuiltin::GD_ARRAY },
		{ "PoolByteArray", Variant::POOL_BYTE_ARRAY, NULL },
		{ "PoolIntArray", Variant::POOL_INT_ARRAY, NULL },
		{ "PoolRealArray", Variant::POOL_REAL_ARRAY, NULL },
		{ "PoolStringArray", Variant::POOL_STRING_ARRAY, NULL },
		{ "PoolVector2Array", Variant::POOL_VECTOR2_ARRAY, NULL },
		{ "PoolVector3Array", Variant::POOL_VECTOR3_ARRAY, NULL },
		{ "PoolColorArray", Variant::POOL_COLOR_ARRAY, NULL },
	};

	const Char_Psn tmp[] = {
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

	memcpy(quickSearch, tmp, sizeof(tmp));
	//TODO::
	// (void*)_VariantCall::type_funcs;
	// auto func =_VariantCall::type_funcs[0].functions[""];
	// func.call()
	//buildIns 添加缺失的，然后在遍历的时候，创建一个另外的VariantMethods的注册表table，
	//然后在里面添加对应lightud转为VariantType那个key，值就是对应的func
	//我们在Variant的直接gettype来取对应func表
	//
	//Variant FuncTable binding

	//Variant binding
	lua_newtable(L);
	{
		typedef struct {
			const char *meta;
			Variant::Operator op;
		} eval;
		static eval evaluates[] = {
			{ "__eq", Variant::OP_EQUAL },
			{ "__add", Variant::OP_ADD },
			{ "__sub", Variant::OP_SUBTRACT },
			{ "__mul", Variant::OP_MULTIPLY },
			{ "__div", Variant::OP_DIVIDE },
			{ "__mod", Variant::OP_MODULE },
			{ "__lt", Variant::OP_LESS },
			{ "__le", Variant::OP_LESS_EQUAL },
			{ "__unm", Variant::OP_NEGATE }
		};

		for (int idx = 0; idx < sizeof(evaluates) / sizeof(evaluates[0]); idx++) {
			eval &ev = evaluates[idx];
			lua_pushstring(L, ev.meta);
			lua_pushinteger(L, ev.op);
			lua_pushcclosure(L, meta_builtins__evaluate, 1);
			lua_rawset(L, -3);
		}

		static luaL_Reg meta_methods[] = {
			//{ "__index", meta_builtins__index },
			{ "__tostring", meta_builtins__tostring },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}

	int len = sizeof(buildIns) / sizeof(BulitinTypes);
	for (int i = 0; i < len; i++) {
		if (buildIns[i].ptr != NULL) {
			lua_pushlightuserdata(L, buildIns[i].ptr);
			shallow_copy(L, -2);
			lua_pushvalue(L, -1);
			//==
			lua_setfield(L, -2, "__index");
			lua_pushlightuserdata(L, (void *)&LuaBindingHelper::TABLE_TYPE);
			//==
			lua_pushvalue(L, -3);
			lua_rawset(L, -3);
			lua_rawset(L, LUA_REGISTRYINDEX);
		}
	}
	lua_pop(L, 1);

	lua_getfield(L, LUA_GLOBALSINDEX, "GD");
	for (int i = 0; i < len; i++) {
		lua_newtable(L);
		lua_newtable(L);
		{
			lua_pushstring(L, "__call");
			lua_pushinteger(L, buildIns[i].vt);
			lua_pushstring(L, buildIns[i].type);
			lua_pushcclosure(L, LuaBuiltin::meta_bultins__call, 2);
			lua_rawset(L, -3);
		}
		lua_setmetatable(L, -2);

		//methods
		for (auto E = _VariantCall::type_funcs[buildIns[i].vt].functions.front(); E != NULL; E = E->next()) {
			lua_pushlightuserdata(L, &E->value());
			lua_pushcclosure(L, l_builtins_methods__wrapper, 1);
			if (buildIns[i].ptr != NULL) {
				lua_pushlightuserdata(L, (void *)buildIns[i].ptr);
				lua_rawget(L, LUA_REGISTRYINDEX);
				lua_pushstring(L, String(E->key()).ascii().get_data());
				lua_pushvalue(L, -3);
				lua_rawset(L, -3);
				lua_pop(L, 1);
			}
			lua_setfield(L, -2, String(E->key()).ascii().get_data());
		}
		//const value
		for (auto E = _VariantCall::constant_data[buildIns[i].vt].value.front(); E != NULL; E = E->next()) {
			lua_pushinteger(L, E->value());
			lua_setfield(L, -2, String(E->key()).ascii().get_data());
		}
		//const variant
		for (auto E = _VariantCall::constant_data[buildIns[i].vt].variant_value.front(); E != NULL; E = E->next()) {
			l_push_variant(L, E->value());
			lua_setfield(L, -2, String(E->key()).ascii().get_data());
		}
		lua_setfield(L, -2, buildIns[i].type);
	}
	lua_pop(L, 1);
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

int LuaBuiltin::meta_builtins__evaluate(lua_State *L) {
	Variant::Operator op = (Variant::Operator)lua_tointeger(L, lua_upvalueindex(1));

	Variant var1;
	l_get_variant(L, 1, var1);

	Variant var2;
	l_get_variant(L, 2, var2);

	Variant ret;
	bool valid = false;
	Variant::evaluate(op, var1, var2, ret, valid);
	if (valid) {

		l_push_variant(L, ret);
		return 1;
	}
	return 0;
}

int LuaBuiltin::meta_builtins__tostring(lua_State *L) {
	Variant var;
	l_get_variant(L, 1, var);
	lua_pushstring(L, (var.operator String()).utf8().get_data());
	return 1;
}
int LuaBuiltin::meta_builtins__index(lua_State *L) {
	Variant var;
	l_get_variant(L, 1, var);
	Variant value;

	const char *index_name = lua_tostring(L, 2);

	if (lua_getmetatable(L, 1)) {
		lua_getfield(L, -1, index_name);
		if (!lua_isnil(L, -1)) {
			//2.is a methed just return
			return 1;
		}
	}
	return 0;
}

int LuaBuiltin::l_builtins_methods__wrapper(lua_State *L) {
	_VariantCall::FuncData *p_func = (_VariantCall::FuncData *)lua_touserdata(L, lua_upvalueindex(1));
	int top = lua_gettop(L);

	Variant var;
	Variant ret;
	l_get_variant(L, 1, var);
	Variant::CallError err;
	err.error = Variant::CallError::CALL_OK;
	if (top == 1) {
		p_func->call(ret, var, NULL, 0, err);
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
		p_func->call(ret, var, (const Variant **)(args), top - 1, err);
		memdelete_arr(vars);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	}
	l_method_error(L, err);
	return 0;
}

Variant LuaBuiltin::l_get_vector2(lua_State *L, int idx) {
	lua_getfield(L, idx, "x");
	auto x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "y");
	auto y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Vector2(x, y);
}
Variant LuaBuiltin::l_get_rect2(lua_State *L, int idx) {
	lua_getfield(L, idx, "position");
	auto position = l_get_vector2(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "size");
	auto size = l_get_vector2(L, -1);
	lua_pop(L, 1);
	return Rect2(position, size);
}
Variant LuaBuiltin::l_get_vector3(lua_State *L, int idx) {
	lua_getfield(L, idx, "x");
	auto x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "y");
	auto y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "z");
	auto z = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Vector3(x, y, z);
}
Variant LuaBuiltin::l_get_transform2d(lua_State *L, int idx) {
	if (idx < 0) {
		idx = lua_gettop(L) + idx + 1;
	}
	lua_pushinteger(L, 0);
	lua_rawget(L, idx);
	Vector2 vec0 = l_get_vector2(L, -1);
	lua_pop(L, 1);

	lua_pushinteger(L, 1);
	lua_rawget(L, idx);
	Vector2 vec1 = l_get_vector2(L, -1);
	lua_pop(L, 1);

	lua_pushinteger(L, 2);
	lua_rawget(L, idx);
	Vector2 vec2 = l_get_vector2(L, -1);
	lua_pop(L, 1);
	return Transform2D(vec0.x, vec0.y, vec1.x, vec1.y, vec2.x, vec2.y);
}
Variant LuaBuiltin::l_get_Plane(lua_State *L, int idx) {
	lua_getfield(L, idx, "normal");
	Vector3 normal = l_get_vector3(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "d");
	auto d = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Plane(normal, d);
}
Variant LuaBuiltin::l_get_quat(lua_State *L, int idx) {
	lua_getfield(L, idx, "x");
	auto x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "y");
	auto y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "z");
	auto z = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "w");
	auto w = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Quat(x, y, z, w);
}
Variant LuaBuiltin::l_get_aabb(lua_State *L, int idx) {
	lua_getfield(L, idx, "position");
	auto position = l_get_vector3(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "size");
	auto size = l_get_vector3(L, -1);
	lua_pop(L, 1);
	return AABB(position, size);
}
Variant LuaBuiltin::l_get_basis(lua_State *L, int idx) {
	if (idx < 0) {
		idx = lua_gettop(L) + idx + 1;
	}
	lua_pushinteger(L, 0);
	lua_rawget(L, idx);
	Vector3 vec0 = l_get_vector3(L, -1);
	lua_pop(L, 1);

	lua_pushinteger(L, 1);
	lua_rawget(L, idx);
	Vector3 vec1 = l_get_vector3(L, -1);
	lua_pop(L, 1);

	lua_pushinteger(L, 2);
	lua_rawget(L, idx);
	Vector3 vec2 = l_get_vector3(L, -1);
	lua_pop(L, 1);
	return Basis(vec0, vec1, vec2);
}
Variant LuaBuiltin::l_get_transform(lua_State *L, int idx) {
	lua_getfield(L, idx, "position");
	auto basis = l_get_basis(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "origin");
	auto origin = l_get_vector3(L, -1);
	lua_pop(L, 1);
	return Transform(basis, origin);
}
Variant LuaBuiltin::l_get_color(lua_State *L, int idx) {
	lua_getfield(L, idx, "r");
	auto r = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "g");
	auto g = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "b");
	auto b = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "a");
	auto a = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Color(r, g, b, a);
}
Variant LuaBuiltin::l_get_dict(lua_State *L, int idx) {
	Dictionary dict;
	if (idx < 0) {
		idx = lua_gettop(L) + idx + 1;
	}
	/* table is in the stack at index 't' */
	lua_pushnil(L); /* first key */
	while (lua_next(L, idx) != 0) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		Variant v;
		Variant k;
		l_get_variant(L, -2, v);
		l_get_variant(L, -1, k);
		dict[k] = v;
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 1);
	}
	return dict;
}
Variant LuaBuiltin::l_get_array(lua_State *L, int idx) {
	Array arr;
	if (idx < 0) {
		idx = lua_gettop(L) + idx + 1;
	}
	/* table is in the stack at index 't' */
	lua_pushnil(L); /* first key */
	while (lua_next(L, idx) != 0) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		Variant tmp;
		l_get_variant(L, -2, tmp);
		arr.push_back(tmp);
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 1);
	}
	return arr;
}

void LuaBuiltin::l_push_vector2_type(lua_State *L, const Vector2 &var) {
	lua_newtable(L);
	lua_pushnumber(L, var.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, var.y);
	lua_setfield(L, -2, "y");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_VECTOR2);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}

void LuaBuiltin::l_push_rect2_type(lua_State *L, const Rect2 &var) {
	lua_newtable(L);
	l_push_vector2_type(L, var.position);
	lua_setfield(L, -2, "position");
	l_push_vector2_type(L, var.size);
	lua_setfield(L, -2, "size");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_RECT2);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_vector3_type(lua_State *L, const Vector3 &var) {
	lua_newtable(L);
	lua_pushnumber(L, var.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, var.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, var.z);
	lua_setfield(L, -2, "z");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_VECTOR3);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_transform2d_type(lua_State *L, const Transform2D &var) {
	lua_newtable(L);
	lua_pushinteger(L, 0);
	l_push_vector2_type(L, var.elements[0]);
	lua_rawset(L, -3);
	lua_pushinteger(L, 1);
	l_push_vector2_type(L, var.elements[1]);
	lua_rawset(L, -3);
	lua_pushinteger(L, 2);
	l_push_vector2_type(L, var.elements[2]);
	lua_rawset(L, -3);
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_TRANSFORM2D);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_plane_type(lua_State *L, const Plane &var) {
	lua_newtable(L);
	l_push_vector3_type(L, var.normal);
	lua_setfield(L, -2, "normal");
	lua_pushnumber(L, var.d);
	lua_setfield(L, -2, "d");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_PLANE);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_quat_type(lua_State *L, const Quat &var) {
	lua_newtable(L);
	lua_pushnumber(L, var.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, var.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, var.z);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, var.w);
	lua_setfield(L, -2, "w");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_QUAT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_aabb_type(lua_State *L, const AABB &var) {
	lua_newtable(L);
	l_push_vector3_type(L, var.position);
	lua_setfield(L, -2, "position");
	l_push_vector3_type(L, var.size);
	lua_setfield(L, -2, "size");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_AABB);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_basis_type(lua_State *L, const Basis &var) {
	lua_newtable(L);
	lua_pushinteger(L, 0);
	l_push_vector3_type(L, var.elements[0]);
	lua_rawset(L, -3);
	lua_pushinteger(L, 1);
	l_push_vector3_type(L, var.elements[1]);
	lua_rawset(L, -3);
	lua_pushinteger(L, 2);
	l_push_vector3_type(L, var.elements[2]);
	lua_rawset(L, -3);
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_BASIS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_transform_type(lua_State *L, const Transform &var) {
	lua_newtable(L);
	l_push_basis_type(L, var.basis);
	lua_setfield(L, -2, "basis");
	l_push_vector3_type(L, var.origin);
	lua_setfield(L, -2, "origin");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_TRANSFORM);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_color_type(lua_State *L, const Color &var) {
	lua_newtable(L);
	lua_pushnumber(L, var.r);
	lua_setfield(L, -2, "r");
	lua_pushnumber(L, var.g);
	lua_setfield(L, -2, "g");
	lua_pushnumber(L, var.b);
	lua_setfield(L, -2, "b");
	lua_pushnumber(L, var.a);
	lua_setfield(L, -2, "a");
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_COLOR);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_dict_type(lua_State *L, const Dictionary &var) {
	lua_newtable(L);
	auto k = var.next(NULL);
	while (k) {
		l_push_variant(L, k);
		l_push_variant(L, var[k]);
		lua_rawset(L, -3);
		k = var.next(k);
	}
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_DICTIONARY);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}
void LuaBuiltin::l_push_array_type(lua_State *L, const Array &var) {
	lua_newtable(L);
	for (int i = 0; i < var.size(); i++) {
		lua_pushinteger(L, i);
		l_push_variant(L, var[i]);
		lua_rawset(L, -3);
	}
	lua_pushlightuserdata(L, (void *)&LuaBuiltin::GD_ARRAY);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}