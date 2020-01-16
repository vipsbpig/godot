#include "luabinding_helper.h"
#include "../debug.h"
#include "../luascript.h"
#include "../luascript_instance.h"
#include "luabuiltin.h"
#include "scene/main/node.h"

static Variant *luaL_checkvariant(lua_State *L, int idx) {
	void *ptr = luaL_checkudata(L, idx, "LuaVariant");
	return *((Variant **)ptr);
}

static LuaScript *luaL_getscript(lua_State *L, int idx) {
	lua_pushstring(L, ".c_script");
	lua_rawget(L, idx);
	LuaScript *ptr = (LuaScript *)lua_touserdata(L, -1);
	lua_pop(L, 1);

	return ptr;
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

void l_push_variant(lua_State *L, const Variant &var) {
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

			//TODO::LuaInstance(?)
			//TODO::ScritpInstance
			Object **ud = NULL;
			ud = (Object **)lua_newuserdata(L, sizeof(Object *));
			*ud = obj;
			luaL_getmetatable(L, "LuaObject");
			lua_setmetatable(L, -2);

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
			print_format("unknow Type:%s", Variant::get_type_name(var.get_type()).utf8().get_data());
			break;
	}
}

void l_push_bulltins_type(lua_State *L, const Variant &var) {
	print_format("builtIn Type:%s", Variant::get_type_name(var.get_type()).utf8().get_data());
	Variant **ptr = (Variant **)lua_newuserdata(L, sizeof(Variant *));
	*ptr = memnew(Variant);
	**ptr = var;
	luaL_getmetatable(L, "LuaVariant");
	lua_setmetatable(L, -2);
}

void l_get_variant(lua_State *L, int idx, Variant &var) {
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

LuaBindingHelper::LuaBindingHelper() :
		L(NULL) {
}

int LuaBindingHelper::l_print(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	print_line(str);
	return 0;
}

int LuaBindingHelper::l_extends(lua_State *L) {
	LuaScript *p_script = (LuaScript *)lua_touserdata(L, lua_upvalueindex(1));
	if (lua_istable(L, -1)) {
		lua_pushstring(L, ".clsinfo");
		lua_rawget(L, -2);
		const ClassDB::ClassInfo *cls = (ClassDB::ClassInfo *)lua_touserdata(L, -1);
		if (cls == NULL) {
			luaL_error(L, "Extends Wrong Type");
			return 0;
		}
		const char *base = String(cls->name).utf8().get_data();
		print_format("l_extends from:%s %d script:%d", base, cls, p_script);
		p_script->cls = cls;

		lua_newtable(L);
		lua_pushlightuserdata(L, p_script);
		lua_setfield(L, -2, ".c_script");

		luaL_getmetatable(L, "LuaScript");
		lua_setmetatable(L, -2);
		l_ref_luascript(L, (void *)p_script);
		return 1;
	}
	if (lua_isstring(L, -1)) {
		//TODO::如果是字符串就从路径加载脚本去继承来写
		//现在临时用Object来代替
		print_line("Should extends from a res.Now Only extends from Object");
		const ClassDB::ClassInfo *cls = &ClassDB::classes["Object"];
		const char *base = String(cls->name).utf8().get_data();
		print_format("l_extends from:%s", base);
		p_script->cls = cls;
		lua_newtable(L);
		lua_pushlightuserdata(L, p_script);
		lua_setfield(L, -2, ".c_script");

		luaL_getmetatable(L, "LuaScript");
		lua_setmetatable(L, -2);
		return 1;
	}
	return 0;
}

void LuaBindingHelper::bind_script_function(const char *name, void *p_script, lua_CFunction fn) {
	lua_pushlightuserdata(L, p_script);
	lua_pushcclosure(L, fn, 1);
	lua_setglobal(L, name);
}

void LuaBindingHelper::unbind_script_function(const char *name) {
	lua_pushnil(L);
	lua_setglobal(L, name);
}

int LuaBindingHelper::create_user_data(lua_State *L) {
	const ClassDB::ClassInfo *cls = (ClassDB::ClassInfo *)lua_touserdata(L, lua_upvalueindex(1));

	Object **ud = NULL;
	ud = (Object **)lua_newuserdata(L, sizeof(Object *));
	Object *object = cls->creation_func();
	print_format("Name %s call new object_ptr:%d ", object->get_class().utf8().get_data(), object);

	*ud = object;
	luaL_getmetatable(L, "LuaObject");
	lua_setmetatable(L, -2);

	//const StringName *key = cls->method_map.next(NULL);
	//    while (key) {
	//        print_format("-- methods:%s", String(*key).utf8().get_data() );
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
		print_format("%s unreferenct by lua gc.", toString.utf8().get_data());
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
	print_format("meta__index: %s call %s", obj->get_class().utf8().get_data(), String(index_name).utf8().get_data());

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
	//TODO:: scriptinstance
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;

	Variant key, value;
	l_get_variant(L, 2, key);
	l_get_variant(L, 3, value);

	bool valid = false;
	obj->set(key, value, &valid);
	if (!valid)
		luaL_error(L, "Unable to set field: '%s'", ((String)key).utf8().get_data());
	return 0;
}

int LuaBindingHelper::meta_bultins__evaluate(lua_State *L) {
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
	Variant *var = luaL_checkvariant(L, 1);
	print_format("var:%d gc", var);
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
	Variant *var = luaL_checkvariant(L, 1);

	Variant key, value;
	l_get_variant(L, 2, key);
	l_get_variant(L, 3, value);

	bool valid = false;
	var->set(key, value, &valid);
	if (!valid)
		luaL_error(L, "Unable to set field: '%s'", ((String)key).utf8().get_data());

	return 0;
}
int LuaBindingHelper::meta_bultins__pairs(lua_State *L) {
	Variant &var = *luaL_checkvariant(L, 1);
	Variant::Type vt = var.get_type();
	switch (vt) {
		case Variant::DICTIONARY:
		case Variant::ARRAY:
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
			lua_pushcclosure(L, l_builtins_iterator, 0);
			l_push_bulltins_type(L, var);
			lua_pushnil(L);
			return 3;
		default:
			luaL_error(L, "Cannot pairs an %s value", var.get_type_name(var.get_type()).utf8().get_data());
			break;
	}
	return 0;
}

int LuaBindingHelper::l_bultins_caller_wrapper(lua_State *L) {
	print_format("l_bultins_caller_wrapper");
	const char *key = luaL_checkstring(L, lua_upvalueindex(1));
	int top = lua_gettop(L);

	Variant *var = luaL_checkvariant(L, 1);
	Variant::CallError err;
	Variant ret;

	if (top == 1) {

		ret = var->call(key, NULL, 0, err);
	} else {

		Variant *vars = memnew_arr(Variant, top - 1);
		Variant *args[128];
		for (int idx = 2; idx <= top; idx++) {
			Variant &var = vars[idx - 2];
			args[idx - 2] = &var;
			l_get_variant(L, idx, var);
		}
		ret = var->call(key, (const Variant **)(args), top - 1, err);
		memdelete_arr(vars);
	}
	switch (err.error) {
		case Variant::CallError::CALL_OK:
			l_push_variant(L, ret);
			return 1;
		case Variant::CallError::CALL_ERROR_INVALID_METHOD:
			luaL_error(L, "Invalid method '%s'", key);
			break;
		case Variant::CallError::CALL_ERROR_INVALID_ARGUMENT:
			luaL_error(L, "Invalid argument to call '%s'", key);
			break;
		case Variant::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
			luaL_error(L, "Too many arguments to call '%s'", key);
			break;
		case Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
			luaL_error(L, "Too few arguments to call '%s'", key);
			break;
		case Variant::CallError::CALL_ERROR_INSTANCE_IS_NULL:
			luaL_error(L, "Instance is null");
			break;
	}
	return 0;
}

//TO BE CONFIRM
int LuaBindingHelper::l_builtins_iterator(lua_State *L) {
	Variant &var = *luaL_checkvariant(L, 1);
	if (var.get_type() == Variant::DICTIONARY) {

		Variant k;
		const Variant *key = NULL;
		const Dictionary &dict = var.operator Dictionary();
		if (!lua_isnil(L, 2)) {
			l_get_variant(L, 2, k);
			key = &k;
		}
		key = dict.next(key);
		if (key != NULL) {
			l_push_variant(L, *key);
			l_push_variant(L, dict[*key]);
			return 2;
		}
	} else {

		int idx = 0;
		if (!lua_isnil(L, 2))
			idx = luaL_optinteger(L, 2, 0) + 1;

		bool r_valid = false;
		Variant value = var.get(idx, &r_valid);
		if (r_valid) {
			lua_pushinteger(L, idx);
			l_push_variant(L, value);
			return 2;
		}
	}
	return 0;
}

int LuaBindingHelper::meta_script__gc(lua_State *L) {
	print_format("meta_script__gc nothing to do");
	return 0;
}

int LuaBindingHelper::meta_script__tostring(lua_State *L) {
	print_format("meta_script__tostring.");
	LuaScript *p_script = luaL_getscript(L, 1);
	auto var = p_script->get_instance_base_type();
	l_push_variant(L, var);
	return 1;
}

int LuaBindingHelper::meta_script__index(lua_State *L) {
	LuaScript *p_script = luaL_getscript(L, 1);
	StringName index_name = lua_tostring(L, 2);
	const char *base = String(p_script->cls->name).utf8().get_data();
	print_format("meta_script__index:%s base:%s %d script:%d", lua_tostring(L, 2), base, p_script->cls, p_script);
	if (p_script->properties_default_value.has(index_name)) {
		l_push_variant(L, p_script->properties_default_value[index_name]);
		return 1;
	} else {
		lua_rawget(L, 1);
		return 1;
	}
	return 0;
}

int LuaBindingHelper::meta_script__newindex(lua_State *L) {
	LuaScript *p_script = luaL_getscript(L, 1);
	StringName index_name = lua_tostring(L, 2);
	const char *base = String(p_script->cls->name).utf8().get_data();
	int idx = 3;
	int t = lua_type(L, idx);
	print_format("meta_script__newindex:%slua_typ:%d  base:%s script:%d lua_t:%d", lua_tostring(L, 2), t, base, p_script->cls, p_script);
	if (LUA_TNONE == t || LUA_TTHREAD == t || LUA_TLIGHTUSERDATA == t || LUA_TTABLE == t) {
		p_script->add_lua_property_type(index_name, t);
		lua_rawset(L, 1);
	} else if (LUA_TFUNCTION == t) {
		p_script->add_lua_method(index_name);
		lua_rawset(L, 1);
	} else {
		Variant var;
		l_get_variant(L, idx, var);
		p_script->add_property_default_value(index_name, var);
	}

	return 0;
}

int LuaBindingHelper::l_script_caller_wrapper(lua_State *L) {
	print_format("l_script_caller_wrapper");
	return 0;
}
void LuaBindingHelper::l_ref_luascript(lua_State *L, void *object) {
	LuaScript *p_script = (LuaScript *)object;
	lua_pushstring(L, "lua_scripts");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	p_script->lua_ref = luaL_ref(L, -2);
	lua_pop(L, 1);
	print_format("l_ref_luascript:%d s:%d", p_script->lua_ref, p_script);
}

void LuaBindingHelper::l_push_luascript_ref(lua_State *L, int ref) {
	lua_pushstring(L, "lua_scripts");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref);
	lua_remove(L, -2);
}

void LuaBindingHelper::l_unref_luascript(void *object) {
	LuaScript *p_script = (LuaScript *)object;
	print_format("l_unref_luascript:%d s:%d", p_script->lua_ref, p_script);

	lua_pushstring(L, "lua_scripts");
	lua_rawget(L, LUA_REGISTRYINDEX);
	luaL_unref(L, -1, p_script->lua_ref);
	lua_pop(L, 1);
}

int LuaBindingHelper::pcall_callback_err_fun(lua_State *L) {
	lua_Debug debug = {};
	int ret = lua_getstack(L, 2, &debug); // 0是pcall_callback_err_fun自己, 1是error函数, 2是真正出错的函数.
	lua_getinfo(L, "Sln", &debug);

	String errmsg = lua_tostring(L, -1);
	lua_pop(L, 1);
	String msg = debug.short_src + String(":line ") + itos(debug.currentline) + "\n";
	if (debug.name != 0) {
		msg += String("(") + debug.namewhat + " " + debug.name + ")";
	}
	print_error(errmsg);
	lua_pushstring(L, msg.utf8().get_data());
	return 1;
}

Variant LuaBindingHelper::instance_call(ScriptInstance *p_instance, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	LuaScriptInstance *p_si = (LuaScriptInstance *)p_instance;
	print_format("instance_call: script:%d ", p_si->script);
	Variant var;
	//get class
	l_push_luascript_ref(L, p_si->script->lua_ref);
	//get function
	lua_pushstring(L, String(p_method).utf8().get_data());
	lua_rawget(L, -2);
	if (lua_isfunction(L, -1)) {
		//args
		for (int i = 0; i < p_argcount; i++) {
			l_push_variant(L, *p_args[i]);
		}
		//error
		lua_pushcfunction(L, pcall_callback_err_fun);
		//pcall
		//5.3可以换成pcallk
		if (lua_pcall(L, p_argcount, 1, -1) == 0) {
			l_get_variant(L, -1, var);
		} else {
			r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
		}
	} else {
		print_format("script cannot get function:%s", String(p_method).utf8().get_data());
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	}
	r_error.argument = p_argcount;
	lua_settop(L, 0);
	return var;
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
	// if (!(String(cls->name) == "Object" || String(cls->name) == "Node" || String(cls->name) == "_OS" || String(cls->name) == "Node2D"))
	// 	return;

	printf("regist:[%s:%s]\n", String(cls->name).utf8().get_data(), String(cls->inherits).utf8().get_data());

	CharString s = String(cls->name).utf8();
	const char *typeName = s.get_data();

	lua_getfield(L, LUA_GLOBALSINDEX, "GD");

	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, typeName);

	lua_pushlightuserdata(L, (void *)cls);
	lua_pushcclosure(L, create_user_data, 1);
	lua_setfield(L, -2, "new");

	lua_pushlightuserdata(L, (void *)cls);
	lua_setfield(L, -2, ".clsinfo");

	lua_pop(L, 2);
}

void LuaBindingHelper::regitser_builtins(lua_State *L) {
	LuaBuiltin::regitser_builtins(L);
}

void LuaBindingHelper::initialize() {
	L = luaL_newstate();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_debug(L);
	lua_settop(L, 0);

	//global function
	globalbind();

	//GD namespace
	lua_newtable(L);
	lua_setfield(L, LUA_GLOBALSINDEX, "GD");

	//hidden script space
	lua_pushstring(L, "lua_scripts");
	lua_newtable(L);
	/* make weak value metatable for ubox table to allow userdata to be
       garbage-collected */
	// lua_newtable(L);
	// lua_pushliteral(L, "__mode");
	// lua_pushliteral(L, "k");
	// lua_rawset(L, -3); /* stack: string ubox mt */
	// lua_setmetatable(L, -2);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//Object binding
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
	}
	lua_pop(L, 1);

	//Variant binding
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
			{ "__unm", Variant::OP_NEGATE }
		};

		for (int idx = 0; idx < sizeof(evaluates) / sizeof(evaluates[0]); idx++) {
			eval &ev = evaluates[idx];
			lua_pushstring(L, ev.meta);
			lua_pushinteger(L, ev.op);
			lua_pushcclosure(L, meta_bultins__evaluate, 1);
			lua_rawset(L, -3);
		}

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
	lua_pop(L, 1);

	//LuaScript binding
	luaL_newmetatable(L, "LuaScript");
	{
		static luaL_Reg meta_methods[] = {
			{ "__gc", meta_script__gc },
			{ "__index", meta_script__index },
			{ "__newindex", meta_script__newindex },
			{ "__tostring", meta_script__tostring },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}
	lua_pop(L, 1);

	// register class
	{
		const StringName *key = ClassDB::classes.next(NULL);
		while (key) {
			const ClassDB::ClassInfo *cls = ClassDB::classes.getptr(*key);
			register_class(L, cls);
			key = ClassDB::classes.next(key);
		}
	}

	//regise builtin
	regitser_builtins(L);
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
