#include "luabinding_helper.h"
#include "../debug.h"
#include "scene/main/node.h"

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
	print_format("meta__gc");

	if (obj->is_class_ptr(Reference::get_class_ptr_static())) {
		Reference *ref = Object::cast_to<Reference>(obj);
		String toString = "[" + obj->get_class() + ":" + itos(obj->get_instance_id()) + "]";
		print_format("%s unreferenct by lua gc.", toString.ascii().get_data());
		if (ref->unreference())
			memdelete(ref);
		return 0;
	}

	if (obj->is_class("Node")) {
		Node *node = Object::cast_to<Node>(obj);
		if (!node->is_inside_tree()) {
			print_format("DELETED!node not in tree");
			memdelete(obj);
			//SceneTree::get_singleton()->queue_delete(obj);
			return 0;
		}
		return 0;
	}
	// lua_pushnil(L);
	// lua_setmetatable(L, 1);
	print_format("Is Object just delete");
	memdelete(obj);
	return 0;
}

int LuaBindingHelper::meta__tostring(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	String toString = "[" + obj->get_class() + ":" + itos(obj->get_instance_id()) + "]";
	lua_pushstring(L, String(Variant(obj)).utf8().get_data());
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
		lua_pushcclosure(L, l_methodbind_wrapper, 1);
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
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;

	Variant ret;
	Variant::CallError err;

	int top = lua_gettop(L);
	if (top >= 2) {

		Variant *vars = memnew_arr(Variant, top - 1);
		Variant *args[128];
		for (int idx = 2; idx <= top; idx++) {

			Variant &var = vars[idx - 2];
			args[idx - 2] = &var;
			l_get_variant(L, idx, var);
		}
		ret = mb->call(obj, (const Variant **)args, top - 1, err);
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
		case Variant::NIL:
			lua_pushnil(L);
			break;
		case Variant::BOOL:
			lua_pushboolean(L, ((bool)var) ? 1 : 0);
			break;
		case Variant::INT:
			lua_pushinteger(L, (int)var);
			break;
		case Variant::REAL:
			lua_pushnumber(L, (double)var);
			break;
		case Variant::STRING:
			lua_pushstring(L, ((String)var).utf8().get_data());
			break;
		case Variant::OBJECT: {
			//TODO::
			Object *obj = var;
			if (obj == NULL) {
				lua_pushnil(L);
				break;
			}

			lua_pushstring(L, "gdlua_ubox");
			lua_rawget(L, LUA_REGISTRYINDEX);
			{
				//TODO::LuaInstance(?)
				//TODO::ScritpInstance
				Object **ud = NULL;
				ud = (Object **)lua_newuserdata(L, sizeof(Object *));
				*ud = obj;
				luaL_getmetatable(L, "LuaObject");
				lua_setmetatable(L, -2);
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -1);
				lua_rawset(L, -4);
				lua_remove(L, 1);
			}
		} break;
		case Variant::VECTOR2:
		case Variant::RECT2:
		case Variant::VECTOR3:
		case Variant::TRANSFORM2D:
		case Variant::PLANE:
		case Variant::QUAT:
		case Variant::AABB:
		case Variant::BASIS:
		case Variant::TRANSFORM:
		case Variant::COLOR:
		case Variant::NODE_PATH:
		case Variant::_RID:
		case Variant::DICTIONARY:
		case Variant::ARRAY:

		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY: {
			l_push_bulltins_type(L, var);
		} break;
		default:
			print_format("unknow Type:%s", Variant::get_type_name(var.get_type()).ascii().get_data());
			break;
	}
}

void LuaBindingHelper::l_push_bulltins_type(lua_State *L, const Variant &var) {
	print_format("builtIn Type:%s", Variant::get_type_name(var.get_type()).ascii().get_data());
	Variant **ptr = (Variant **)lua_newuserdata(L, sizeof(Variant *));
	*ptr = memnew(Variant);
	**ptr = var;
	luaL_getmetatable(L, "LuaVariant");
	lua_setmetatable(L, -2);
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
			void *ud = lua_touserdata(L, idx);
			if (ud != NULL) {
				if (lua_getmetatable(L, idx)) {
					lua_getfield(L, LUA_REGISTRYINDEX, "LuaObject");
					if (lua_rawequal(L, -1, -2)) {

						lua_pop(L, 2);
						Object *obj = *((Object **)ud);
						if (obj->is_class_ptr(Reference::get_class_ptr_static())) {
							Reference *ref = Object::cast_to<Reference>(obj);
							var = Ref<Reference>(ref);
						} else {
							var = obj;
						}
						return;
					}
					lua_pop(L, 1);

					lua_getfield(L, LUA_REGISTRYINDEX, "LuaVariant");
					if (lua_rawequal(L, -1, -2)) {

						lua_pop(L, 2);
						var = **((Variant **)ud);
						return;
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
			}
		} break;
	}
}

int LuaBindingHelper::meta_bultins__evaluate(lua_State *L) {
	print_format("call meta_bultins__evaluate");
	Variant::Operator op = (Variant::Operator)lua_tointeger(L, lua_upvalueindex(1));

	Variant *var1 = luaL_checkvariant(L, 1);

	Variant var2;
	l_get_variant(L, 2, var2);

	Variant ret;
	bool valid = false;
	Variant::evaluate(op, *var1, var2, ret, valid);
	if (valid) {

		l_push_variant(L, ret);
		return 1;
	}
	return 0;
}

int LuaBindingHelper::meta_bultins__gc(lua_State *L) {
	print_format("call meta_bultins__gc");
	Variant *var = luaL_checkvariant(L, 1);
	memdelete(var);

	// lua_pushnil(L);
	// lua_setmetatable(L, 1);
	return 0;
}
int LuaBindingHelper::meta_bultins__tostring(lua_State *L) {
	Variant *var = luaL_checkvariant(L, 1);
	lua_pushstring(L, (var->operator String()).utf8().get_data());
	return 1;
}
int LuaBindingHelper::meta_bultins__index(lua_State *L) {
	print_format("call meta_bultins__index");
	Variant *var = luaL_checkvariant(L, 1);

	Variant key;
	l_get_variant(L, 2, key);

	bool valid = false;
	Variant value = var->get(key, &valid);
	if (valid) {
		l_push_variant(L, value);
		return 1;
	}

	if (lua_type(L, 2) == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_pushcclosure(L, l_bultins_caller_wrapper, 1);
		return 1;
	}
	return 0;
}
int LuaBindingHelper::meta_bultins__newindex(lua_State *L) {
	print_format("call meta_bultins__newindex");
	return 0;
}
int LuaBindingHelper::meta_bultins__pairs(lua_State *L) {
	print_format("call meta_bultins__pairs");
	return 0;
}

int LuaBindingHelper::l_bultins_caller_wrapper(lua_State *L) {
	print_format("l_bultins_caller_wrapper");
	return 0;
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
	if (!(String(cls->name) == "Object" || String(cls->name) == "Node" || String(cls->name) == "_OS" || String(cls->name) == "Node2D"))

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

	luaL_newmetatable(L, "LuaVariant");
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
		};
		//二元运算
		for (int idx = 0; idx < sizeof(evaluates) / sizeof(evaluates[0]); idx++) {

			eval &ev = evaluates[idx];
			lua_pushstring(L, ev.meta);
			lua_pushinteger(L, ev.op);
			lua_pushcclosure(L, meta_bultins__evaluate, 1);
			lua_rawset(L, -3);
		}
		//一元运算
		//{ "__unm", Variant::OP_NEGATE }

		static luaL_Reg meta_methods[] = {
			{ "__gc", meta_bultins__gc },
			{ "__index", meta_bultins__index },
			{ "__newindex", meta_bultins__newindex },
			{ "__tostring", meta_bultins__tostring },
			{ "__pairs", meta_bultins__pairs },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}
	stackDump(L);
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