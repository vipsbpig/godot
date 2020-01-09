#include "luabinding_helper.h"
#include "../debug.h"

LuaBindingHelper::LuaBindingHelper() :
		L(NULL) {
}

int LuaBindingHelper::l_print(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	print_line(str);
	return 0;
}

int LuaBindingHelper::l_extends(lua_State *L) {
	print_format("call %s", "l_extends");
	return 0;
}

int LuaBindingHelper::create_user_data(lua_State *L) {
	const ClassDB::ClassInfo *cls = (ClassDB::ClassInfo *)lua_touserdata(L, lua_upvalueindex(1));

	//const StringName *key = cls->method_map.next(NULL);

	lua_pushstring(L, "gdlua_ubox");
	lua_rawget(L, LUA_REGISTRYINDEX);
	{
		Object **ud = NULL;
		ud = (Object **)lua_newuserdata(L, sizeof(Object *));
		Object *object = cls->creation_func();
		print_format("Name %s call new object_ptr:%d ", object->get_class().ascii().get_data(), object);

		*ud = object;
		luaL_getmetatable(L, "LuaObject");
		lua_setmetatable(L, -2);
		lua_pushvalue(L, -1);
		lua_pushvalue(L, -1);
		lua_rawset(L, -4);
		lua_remove(L, 1);
	}
	//    while (key) {
	//        print_format("-- methods:%s", String(*key).ascii().get_data() );
	//        key = cls->method_map.next(key);
	//    }
	return 1;
}

int LuaBindingHelper::meta__gc(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	String toString = "[" + obj->get_class() + ":" + itos(obj->get_instance_id()) + "]";
	print_format("%s memdeted by lua gc.", toString.ascii().get_data());
	memdelete(obj);
	return 0;
}

int LuaBindingHelper::meta__tostring(lua_State *L) {
	print_format("meta__tostring");
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	String toString = "[" + obj->get_class() + ":" + itos(obj->get_instance_id()) + "]";
	lua_pushstring(L, toString.ascii().get_data());
	return 1;
}

int LuaBindingHelper::meta__index(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	StringName index_name = lua_tostring(L, 2);
	print_format("meta__index: %s call %s", obj->get_class().ascii().get_data(), String(index_name).ascii().get_data());

	//1.如果是变量，压入
	bool success = false;
	Variant variant = obj->get(index_name, &success);
	if (success) {
		l_push_variant(L, variant);
		return 1;
	}
	//2.如果是方法，压入
	MethodBind *mb = ClassDB::get_method(obj->get_class_name(), index_name);
	if (mb != NULL) {
		lua_pushlightuserdata(L, mb);
		lua_pushlightuserdata(L, obj);
		lua_pushcclosure(L, l_methodbind_wrapper, 2);
		return 1;
	}
	return 0;
}

int LuaBindingHelper::meta__newindex(lua_State *L) {
	print_format("call %s", "meta__newindex");
	return 0;
}

int LuaBindingHelper::l_methodbind_wrapper(lua_State *L) {
	MethodBind *mb = (MethodBind *)lua_touserdata(L, lua_upvalueindex(1));
	Object *obj = (Object *)lua_touserdata(L, lua_upvalueindex(2));

	Variant ret;
	Variant::CallError err;

	int top = lua_gettop(L);
	if (top >= 1) {

		Variant *vars = memnew_arr(Variant, top);
		Variant *args[128];
		for (int idx = 1; idx <= top; idx++) {

			Variant &var = vars[idx - 1];
			args[idx - 1] = &var;
			l_get_variant(L, idx, var);
		}
		ret = mb->call(obj, (const Variant **)args, top, err);
		memdelete_arr(vars);
	} else {
		ret = mb->call(obj, NULL, 0, err);
	}
	switch (err.error) {
		case Variant::CallError::CALL_OK:
			l_push_variant(L, ret);
			return 1;
		case Variant::CallError::CALL_ERROR_INVALID_METHOD:
			luaL_error(L, "Invalid method");
			break;
		case Variant::CallError::CALL_ERROR_INVALID_ARGUMENT:
			luaL_error(L, "Invalid arguments");
			break;
		case Variant::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
			luaL_error(L, "Too many arguments");
			break;
		case Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
			luaL_error(L, "Too few arguments");
			break;
		case Variant::CallError::CALL_ERROR_INSTANCE_IS_NULL:
			luaL_error(L, "Instance is null");
			break;
	}
	return 0;
}

void LuaBindingHelper::l_push_variant(lua_State *L, const Variant &var) {
	switch (var.get_type()) {
		case Variant::Type::NIL:
			lua_pushnil(L);
			break;
		case Variant::Type::BOOL:
			lua_pushboolean(L, ((bool)var) ? 1 : 0);
			break;
		case Variant::Type::INT:
			lua_pushinteger(L, (int)var);
			break;
		case Variant::Type::REAL:
			lua_pushnumber(L, (double)var);
			break;
		case Variant::Type::STRING:
			lua_pushstring(L, ((String)var).utf8().get_data());
			break;
		case Variant::Type::OBJECT: {
			//TODO::
			lua_pushstring(L, "TODO Object TYPE");
		} break;
		case Variant::Type::VECTOR2:
		case Variant::Type::RECT2:
		case Variant::Type::VECTOR3:
		case Variant::Type::TRANSFORM2D:
		case Variant::Type::PLANE:
		case Variant::Type::QUAT:
		case Variant::Type::AABB:
		case Variant::Type::BASIS:
		case Variant::Type::TRANSFORM:
		case Variant::Type::COLOR:
		case Variant::Type::NODE_PATH:
		case Variant::Type::_RID:
		case Variant::Type::DICTIONARY:
		case Variant::Type::ARRAY:

		case Variant::Type::POOL_BYTE_ARRAY:
		case Variant::Type::POOL_INT_ARRAY:
		case Variant::Type::POOL_REAL_ARRAY:
		case Variant::Type::POOL_STRING_ARRAY:
		case Variant::Type::POOL_VECTOR2_ARRAY:
		case Variant::Type::POOL_VECTOR3_ARRAY:
		case Variant::Type::POOL_COLOR_ARRAY: {
			l_push_bulltins_type(L, var);
		} break;
		default:
			print_format("unknow Type:", Variant::get_type_name(var.get_type()).ascii().get_data());
			break;
	}
}

void LuaBindingHelper::l_push_bulltins_type(lua_State *L, const Variant &var) {
	//TODO::
	print_format("builtIn Type:", Variant::get_type_name(var.get_type()).ascii().get_data());
	lua_pushstring(L, "TODO BUILTIN TYPE");
}

void LuaBindingHelper::l_get_variant(lua_State *L, int idx, Variant &var) {
	switch (lua_type(L, idx)) {
		case LUA_TNONE:
		case LUA_TNIL:
		case LUA_TTHREAD:
		case LUA_TLIGHTUSERDATA:
		case LUA_TFUNCTION:
			var = Variant();
			break;

		case LUA_TTABLE:
			break;

		case LUA_TBOOLEAN:
			var = (lua_toboolean(L, idx) != 0);
			break;

		case LUA_TNUMBER:
			if (lua_isinteger(L, idx))
				var = Variant((int)lua_tointeger(L, idx));
			else
				var = Variant(lua_tonumber(L, idx));
			break;

		case LUA_TSTRING: {
			String str;
			str.parse_utf8(lua_tostring(L, idx));
			var = Variant(str);
		} break;

		case LUA_TUSERDATA: {

			void *p = lua_touserdata(L, idx);
			if (p != NULL) {
				// 决定是不是Object 或者 Variant ，再进行转换
			}
		} break;
	}
}

void LuaBindingHelper::openLibs(lua_State *L) {
	luaL_Reg lualibs[] = {
		{ "", luaopen_base },
		//{LUA_LOADLIBNAME, luaopen_package},
		//{LUA_TABLIBNAME, luaopen_table},
		//{LUA_IOLIBNAME, luaopen_io},
		//{LUA_OSLIBNAME, luaopen_os},
		//{LUA_STRLIBNAME, luaopen_string},
		//{LUA_MATHLIBNAME, luaopen_math},
		{ LUA_DBLIBNAME, luaopen_debug },
		//{"lua-utf8", luaopen_utf8},
		{ NULL, NULL }
	};

	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		luaL_register(L, lib->name, lib);
	}
}

void LuaBindingHelper::globalbind() {
	lua_pushcfunction(L, l_print);
	lua_setglobal(L, "println");
}

void LuaBindingHelper::register_class(lua_State *L, const ClassDB::ClassInfo *cls) {
	if (!(String(cls->name) == "Object" || String(cls->name) == "Node" || String(cls->name) == "_OS"))

		return;

	//print_format("stack size = %d", lua_gettop(L));
	print_format("regist:[%s:%s]", String(cls->name).ascii().get_data(), String(cls->inherits).ascii().get_data());

	CharString s = String(cls->name).ascii();
	const char *typeName = s.get_data();
	//print_format("0-stack size = %d", lua_gettop(L));

	lua_newtable(L);

	lua_pushvalue(L, -1);
	lua_setglobal(L, typeName);
	//print_format("1-stack size = %d ,push %s in to global", lua_gettop(L),typeName);

	//lua_pushvalue(L,-1);
	lua_pushlightuserdata(L, (void *)cls);
	lua_pushcclosure(L, create_user_data, 1);
	lua_setfield(L, -2, "new");

	lua_pop(L, 1);
}

void LuaBindingHelper::initialize() {
	L = luaL_newstate();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_debug(L);
	lua_settop(L, 0);

	//global function
	globalbind();
	//basic info
	luaL_newmetatable(L, "LuaObject");
	{
		luaL_Reg meta_methods[] = {
			{ "__gc", meta__gc },
			{ "__index", meta__index },
			{ "__newindex", meta__newindex },
			{ "__tostring", meta__tostring },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);

		lua_newtable(L);
		luaL_Reg methods[] = {
			{ "extends", l_extends },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, methods, 0);
		lua_setfield(L, -2, ".methods");
	}
	lua_pop(L, 1);

	//print_format("stack size = %d", lua_gettop(L));

	/* create object ptr -> udata mapping table */
	lua_pushstring(L, "gdlua_ubox");
	lua_newtable(L);
	/* make weak value metatable for ubox table to allow userdata to be
       garbage-collected */
	lua_newtable(L);
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "kv");
	lua_rawset(L, -3); /* stack: string ubox mt */
	lua_setmetatable(L, -2); /* stack: string ubox */
	lua_rawset(L, LUA_REGISTRYINDEX);

	// register class
	{
		const StringName *key = ClassDB::classes.next(NULL);
		while (key) {
			const ClassDB::ClassInfo *cls = ClassDB::classes.getptr(*key);
			register_class(L, cls);
			key = ClassDB::classes.next(key);
		}
	}

	print_format("stack size = %d", lua_gettop(L));

	//===================
	//    LuaTestParent* parent = new LuaTestParent();
	//    auto* child = new LuaTestChild();
	//    child->funcC(parent);

	//    LuaTestParent p;
	//    child->funcD(p);
}

void LuaBindingHelper::uninitialize() {
	lua_close(L);
	L = NULL;
}

Error LuaBindingHelper::script(const String &p_source) {
	ERR_FAIL_NULL_V(L, ERR_DOES_NOT_EXIST);
	luaL_loadstring(L, p_source.utf8());
	if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
		lua_getfield(L, LUA_GLOBALSINDEX, "debug");
		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			return ERR_SCRIPT_FAILED;
		}
		lua_getfield(L, -1, "traceback");
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 2);
			return ERR_SCRIPT_FAILED;
		}
		lua_pushvalue(L, 1); /* pass error message */
		lua_pushinteger(L, 2); /* skip this function and traceback */
		lua_call(L, 2, 1); /* call debug.traceback */
		print_line(lua_tostring(L, -1));
		return ERR_SCRIPT_FAILED;
	}

	return OK;
}
