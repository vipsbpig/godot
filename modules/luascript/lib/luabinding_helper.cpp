#include "luabinding_helper.h"
#include "../luascript.h"
#include "../luascript_instance.h"
#include "luabuiltin.h"
#include "scene/main/node.h"
#include <lualib.h>

static Variant *luaL_checkvariant(lua_State *L, int idx) {
	void *ptr = lua_touserdata(L, idx);
	return *((Variant **)ptr);
}

static LuaScript *luaL_getscript(lua_State *L, int idx) {
	lua_pushstring(L, ".c_script");
	lua_rawget(L, idx);
	LuaScript *ptr = (LuaScript *)lua_touserdata(L, -1);
	lua_pop(L, 1);

	return ptr;
}
static LuaScriptInstance *luaL_getinstance(lua_State *L, int idx) {
	lua_pushstring(L, ".c_instance");
	lua_rawget(L, idx);
	LuaScriptInstance *ptr = (LuaScriptInstance *)lua_touserdata(L, -1);
	lua_pop(L, 1);

	return ptr;
}

void l_method_error(lua_State *L, const Variant::CallError &err) {
	switch (err.error) {
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
		default: break;
	}
}

int l_methodbind_wrapper(lua_State *L) {

	MethodBind *mb = (MethodBind *)lua_touserdata(L, lua_upvalueindex(1));
	Object *obj = NULL;
	int t = lua_type(L, 1);
	if (LUA_TTABLE == t) {
		if (lua_getmetatable(L, 1)) {
			lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAINSTANCE);
			lua_rawget(L, LUA_REGISTRYINDEX);
			if (lua_rawequal(L, -1, -2)) {
				LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
				obj = p_instance->get_owner();
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	} else if (LUA_TUSERDATA == t) {
		Object **ud = (Object **)lua_touserdata(L, 1);
		obj = *ud;
	}
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
		Variant &&ret = mb->call(obj, (const Variant **)args, top - 1, err);
		memdelete_arr(vars);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	} else {
		Variant &&ret = mb->call(obj, NULL, 0, err);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	}
	l_method_error(L, err);
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
			//TODO::LuaInstannce->baseref
			// 		if (p_instance) {
			// if (p_instance->base_ref && static_cast<Reference *>(p_instance->owner)->is_referenced()) {
			// 	self = REF(static_cast<Reference *>(p_instance->owner));
			//TODO::ScritpInstance

			LuaBindingHelper::script_pushobject(L, obj);
			if (obj->is_class_ptr(Reference::get_class_ptr_static())) {
				Reference *ref = Object::cast_to<Reference>(obj);
				ref->reference();
			}
		} break;
		case Variant::VECTOR2: {
			LuaBuiltin::l_push_vector2_type(L, var);
			break;
		}
		case Variant::RECT2: {
			LuaBuiltin::l_push_rect2_type(L, var);
			break;
		}
		case Variant::VECTOR3: {
			LuaBuiltin::l_push_vector3_type(L, var);
			break;
		}
		case Variant::TRANSFORM2D: {
			LuaBuiltin::l_push_transform2d_type(L, var);
			break;
		}
		case Variant::PLANE: {
			LuaBuiltin::l_push_plane_type(L, var);
			break;
		}
		case Variant::QUAT: {
			LuaBuiltin::l_push_quat_type(L, var);
			break;
		}
		case Variant::AABB: {
			LuaBuiltin::l_push_aabb_type(L, var);
			break;
		}
		case Variant::BASIS: {
			LuaBuiltin::l_push_basis_type(L, var);
			break;
		}
		case Variant::TRANSFORM: {
			LuaBuiltin::l_push_transform_type(L, var);
			break;
		}
		case Variant::COLOR: {
			LuaBuiltin::l_push_color_type(L, var);
			break;
		}
		case Variant::NODE_PATH:
		case Variant::_RID: {
			l_push_bulltins_type(L, var);
		} break;
		case Variant::DICTIONARY: {
			LuaBuiltin::l_push_dict_type(L, var);
			break;
		}
		case Variant::ARRAY: {
			LuaBuiltin::l_push_array_type(L, var);
			break;
		}
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY: {
			l_push_bulltins_type(L, var);
			break;
		}
		default:
			luaL_error(L, "unknow Type:%s", Variant::get_type_name(var.get_type()).ascii().get_data());
			break;
	}
}

void l_push_bulltins_type(lua_State *L, const Variant &var) {
	Variant **ptr = (Variant **)lua_newuserdata(L, sizeof(Variant *));
	*ptr = memnew(Variant);
	**ptr = var;
	lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAVARIANT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
}

bool is_registry_gd_built(lua_State *L, int idx, const void *p_builtin, Variant (*func)(lua_State *, int), Variant &var) {
	lua_pushlightuserdata(L, (void *)p_builtin);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (lua_rawequal(L, -1, -2)) {
		var = func(L, idx);
		lua_pop(L, 1);
		return true;
	}
	lua_pop(L, 1);
	return false;
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

		case LUA_TTABLE: {

			if (lua_getmetatable(L, idx)) {
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_VECTOR2, LuaBuiltin::l_get_vector2, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_RECT2, LuaBuiltin::l_get_rect2, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_VECTOR3, LuaBuiltin::l_get_vector3, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_TRANSFORM2D, LuaBuiltin::l_get_transform2d, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_PLANE, LuaBuiltin::l_get_Plane, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_QUAT, LuaBuiltin::l_get_quat, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_AABB, LuaBuiltin::l_get_aabb, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_BASIS, LuaBuiltin::l_get_basis, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_TRANSFORM, LuaBuiltin::l_get_transform, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_COLOR, LuaBuiltin::l_get_color, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_DICTIONARY, LuaBuiltin::l_get_dict, var)) return;
				if (is_registry_gd_built(L, idx, &LuaBuiltin::GD_ARRAY, LuaBuiltin::l_get_array, var)) return;

				lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAINSTANCE);
				lua_rawget(L, LUA_REGISTRYINDEX);
				if (lua_rawequal(L, -1, -2)) {
					LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
					var = p_instance->get_owner();
				}
				lua_pop(L, 1);
			}
			break;
		}
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
			var = Variant(lua_tostring(L, idx));
		} break;

		case LUA_TUSERDATA: {
			void *ud = lua_touserdata(L, idx);
			if (ud != NULL) {
				if (lua_getmetatable(L, idx)) {
					lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAOBJECT);
					lua_rawget(L, LUA_REGISTRYINDEX);
					if (lua_rawequal(L, -1, -2)) {
						lua_pop(L, 2);
						if (ud == NULL) {
							var = Variant();
							return;
						}
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

					lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAVARIANT);
					lua_rawget(L, LUA_REGISTRYINDEX);
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

const char *l_get_key(lua_State *L, int idx) {
	return lua_tostring(L, idx);
}

const char LuaBindingHelper::LUAOBJECT = 0;
const char LuaBindingHelper::LUAVARIANT = 1;
const char LuaBindingHelper::LUASCRIPT = 2;
const char LuaBindingHelper::LUAINSTANCE = 3;

const char LuaBindingHelper::GD_CLASS = 4;
const char LuaBindingHelper::WEAK_UBOX = 5;
const char LuaBindingHelper::REF_UBOX = 6;
const char LuaBindingHelper::GD_SCRIPT_REF = 7;
const char LuaBindingHelper::GD_INSTANCE_REF = 8;

LuaBindingHelper::LuaBindingHelper() :
		L(NULL) {
}

int LuaBindingHelper::l_print(lua_State *L) {
	Variant var;
	l_get_variant(L, 1, var);
	print_line(var);
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

		p_script->cls = cls;

		lua_newtable(L);
		lua_pushlightuserdata(L, p_script);
		lua_setfield(L, -2, ".c_script");

		lua_pushlightuserdata(L, (void *)&LUASCRIPT);
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_setmetatable(L, -2);
		l_ref_luascript(L, (void *)p_script);
		return 1;
	}
	if (lua_isstring(L, -1)) {
		//TODO:: if extend form ResPath this should hinrate from that luascript
		return 0;
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
	Object *object = cls->creation_func();
	script_pushobject(L, object);
	return 1;
}
int LuaBindingHelper::script_pushobject(lua_State *L, Object *object) {
	Object **ud;
	lua_pushlightuserdata(L, (void *)&WEAK_UBOX);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushnumber(L, object->get_instance_id());
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TUSERDATA) {
		ud = (Object **)lua_touserdata(L, -1);
		if (*ud == object) {
			lua_replace(L, -2);
			return 1;
		}
	}
	ud = (Object **)lua_newuserdata(L, sizeof(Object *));
	*ud = object;

	lua_pushlightuserdata(L, (void *)&LUAOBJECT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);

	lua_pushnumber(L, object->get_instance_id());
	lua_pushvalue(L, -2);
	lua_rawset(L, -5);
	lua_replace(L, -3);
	lua_pop(L, 1);
	return 1;
}

void LuaBindingHelper::push_strong_ref(lua_State *L, Object *object) {
	script_pushobject(L, object);
	lua_pushlightuserdata(L, (void *)&REF_UBOX);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushnumber(L, object->get_instance_id());
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 2);
};
void LuaBindingHelper::del_strong_ref(lua_State *L, Object *object) {
	lua_pushlightuserdata(L, (void *)&REF_UBOX);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushnil(L);
	lua_pushnumber(L, object->get_instance_id());
	lua_rawset(L, -3);
	lua_pop(L, 1);
};

int LuaBindingHelper::meta_object__gc(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	if (ud == NULL) {
		return 0;
	}
	Object *obj = *ud;
	if (obj == NULL) {
		return 0;
	}
	if (obj->is_class_ptr(Reference::get_class_ptr_static())) {
		Reference *ref = Object::cast_to<Reference>(obj);
		if (ref->unreference()) {
			memdelete(ref);
		}
	}
	return 0;
}
int LuaBindingHelper::meta_object__tostring(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	if (obj != NULL) {
		String toString = "[" + obj->get_class() + ":" + itos(obj->get_instance_id()) + "]";
		lua_pushstring(L, toString.ascii().get_data());
	} else
#ifdef DEBUG_ENABLED
		lua_pushstring(L, "[DELETED Object]");
#else
		return 0;
#endif
	return 1;
}
int LuaBindingHelper::meta_object__index(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	const char *index_name = l_get_key(L, 2);
	if (obj == NULL) {
		luaL_error(L, "Faild To Get %s Form NULL Object", index_name);
		return 0;
	}

	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	{
		lua_pushlightuserdata(L, ClassDB::classes.getptr(obj->get_class_name()));
		lua_rawget(L, -2);
		lua_getfield(L, -1, index_name);

		if (!lua_isnil(L, -1)) {
			//1.is a variant get form getter
			const ClassDB::PropertySetGet *setget = (const ClassDB::PropertySetGet *)lua_touserdata(L, -1);
			if (setget && setget->_getptr) {
				MethodBind *mb = setget->_getptr;
				Variant::CallError err;
				Variant variant = mb->call(obj, NULL, 0, err);
				if (err.error == Variant::CallError::CALL_OK) {
					l_push_variant(L, variant);
					return 1;
				} else {
					l_method_error(L, err);
				}
			}

			//2.is a methed just return
			return 1;
		}
	}
	lua_pop(L, 1);

	//3.call free to delete object
	if (strncmp(index_name, "free", 4) == 0) {
		lua_pushcclosure(L, l_object_free, 0);
		return 1;
	}
	return 0;
}
int LuaBindingHelper::meta_object__newindex(lua_State *L) {

	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	if (obj == NULL) {
		const char *index_name = lua_tostring(L, 2);
		luaL_error(L, "Failed To Set Field :'%s' To NULL Object", index_name);
		return 0;
	}

	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	{

		lua_pushlightuserdata(L, ClassDB::classes.getptr(obj->get_class_name()));
		lua_rawget(L, -2);
		lua_getfield(L, -1, l_get_key(L, 2));

		if (!lua_isnil(L, -1)) {
			//1.is a variant set form setter
			ClassDB::PropertySetGet *setget = (ClassDB::PropertySetGet *)lua_touserdata(L, -1);
			if (setget && setget->_setptr) {
				MethodBind *mb = setget->_setptr;
				Variant::CallError err;
				Variant value;
				l_get_variant(L, 3, value);
				const Variant *arg[1] = { &value };
				Variant variant = mb->call(obj, arg, 1, err);
				if (err.error == Variant::CallError::CALL_OK) {
					return 0;
				} else {
					l_method_error(L, err);
				}
			}
		}
	}
	lua_pop(L, 1);
	return 0;
}

int LuaBindingHelper::l_object_free(lua_State *L) {
	Object **ud = (Object **)lua_touserdata(L, 1);
	if (ud == NULL) {
		return 0;
	}
	Object *obj = *ud;
	memdelete(obj);
	*ud = NULL;
	return 0;
}
void LuaBindingHelper::l_add_reference(Object *p_reference) {
	printf("l_add_reference ref:%s\n", String(Variant(p_reference)).ascii().get_data());
	//push_strong_ref(L, p_reference);
}
bool LuaBindingHelper::l_del_reference(Object *p_reference) {
	printf("l_del_reference ref:%s\n", String(Variant(p_reference)).ascii().get_data());
	//del_strong_ref(L, p_reference);
	return true;
}

int LuaBindingHelper::meta_variants__evaluate(lua_State *L) {
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

int LuaBindingHelper::meta_variants__gc(lua_State *L) {
	Variant *var = luaL_checkvariant(L, 1);
	memdelete(var);
	return 0;
}
int LuaBindingHelper::meta_variants__tostring(lua_State *L) {
	Variant *var = luaL_checkvariant(L, 1);
	lua_pushstring(L, (var->operator String()).utf8().get_data());
	return 1;
}
int LuaBindingHelper::meta_variants__index(lua_State *L) {
	Variant *var = luaL_checkvariant(L, 1);
	Variant value;

	const char *index_name = l_get_key(L, 2);

	bool valid = false;
	bool founded = false;
	const StringName *sn = LuaBuiltin::GetVariantPropStringName(index_name, founded);
	if (founded)
		value = var->get(*sn, &valid);
	else
		value = var->get(index_name, &valid);
	if (valid) {
		l_push_variant(L, value);
		return 1;
	}
	//buildins methods
	lua_getfield(L, LUA_REGISTRYINDEX, "VariantMethods");
	if (lua_isnil(L, -1)) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_replace(L, -3);
		lua_setfield(L, LUA_REGISTRYINDEX, "VariantMethods");
	}
	lua_getfield(L, -1, index_name);
	if (lua_isnil(L, -1)) {
		l_push_stringname(L, index_name);
		lua_pushvalue(L, -1);
		lua_replace(L, -3);
		lua_setfield(L, -3, index_name);
	}
	lua_pushcclosure(L, l_variants_caller_wrapper, 1);

	return 1;
}
int LuaBindingHelper::meta_variants__newindex(lua_State *L) {
	Variant *var = luaL_checkvariant(L, 1);

	Variant value;
	const char *index_name = l_get_key(L, 2);
	l_get_variant(L, 3, value);

	bool valid = false;
	bool founded = false;
	const StringName *sn = LuaBuiltin::GetVariantPropStringName(index_name, founded);
	if (founded)
		var->set(*sn, value, &valid);
	else
		var->set(index_name, value, &valid);

	if (!valid)
		luaL_error(L, "Unable to set field: '%s'", lua_tostring(L, 2));

	return 0;
}
int LuaBindingHelper::meta_variants__pairs(lua_State *L) {
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
			lua_pushcclosure(L, l_variants_iterator, 0);
			l_push_bulltins_type(L, var);
			lua_pushnil(L);
			return 3;
		default:
			luaL_error(L, "Cannot pairs an %s value", var.get_type_name(var.get_type()).ascii().get_data());
			break;
	}
	return 0;
}

int LuaBindingHelper::l_variants_caller_wrapper(lua_State *L) {
	const StringName *key = *(StringName **)lua_touserdata(L, lua_upvalueindex(1));
	int top = lua_gettop(L);

	Variant *var = luaL_checkvariant(L, 1);
	Variant::CallError err;

	if (top == 1) {
		Variant &&ret = var->call(*key, NULL, 0, err);
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
		Variant &&ret = var->call(*key, (const Variant **)(args), top - 1, err);
		memdelete_arr(vars);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	}
	l_method_error(L, err);
	return 0;
}

int LuaBindingHelper::l_variants_iterator(lua_State *L) {
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
	//NOTHING TODO
	return 0;
}

int LuaBindingHelper::meta_script__tostring(lua_State *L) {
	LuaScript *p_script = luaL_getscript(L, 1);
	auto var = p_script->get_instance_base_type();
	l_push_variant(L, var);
	return 1;
}

int LuaBindingHelper::meta_script__index(lua_State *L) {
	LuaScript *p_script = luaL_getscript(L, 1);
	const char *index_name = l_get_key(L, 2);

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
	const char *index_name = l_get_key(L, 2);
	int idx = 3;
	int t = lua_type(L, idx);

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

void LuaBindingHelper::l_ref_luascript(lua_State *L, void *object) {
	LuaScript *p_script = (LuaScript *)object;
	lua_pushlightuserdata(L, (void *)&GD_SCRIPT_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	p_script->lua_ref = luaL_ref(L, -2);
	lua_pop(L, 1);
}

void LuaBindingHelper::l_push_luascript_ref(lua_State *L, int ref) {
	lua_pushlightuserdata(L, (void *)&GD_SCRIPT_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref);
	lua_remove(L, -2);
}

void LuaBindingHelper::l_unref_luascript(void *object) {
	LuaScript *p_script = (LuaScript *)object;
	lua_pushlightuserdata(L, (void *)&GD_SCRIPT_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	luaL_unref(L, -1, p_script->lua_ref);
	lua_pop(L, 1);
}

int LuaBindingHelper::meta_instance__gc(lua_State *L) {
	//NOTHING TODO
	return 0;
}
int LuaBindingHelper::meta_instance__tostring(lua_State *L) {
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	lua_pushstring(L, String(Variant(p_instance->get_owner())).ascii().get_data());
	return 1;
}
int LuaBindingHelper::meta_instance__index(lua_State *L) {
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	const char *index_name = l_get_key(L, 2);
	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	{
		lua_pushlightuserdata(L, ClassDB::classes.getptr(p_instance->owner->get_class_name()));
		lua_rawget(L, -2);
		lua_getfield(L, -1, index_name);
		if (!lua_isnil(L, -1)) {

			//1.is a variant get form getter
			const ClassDB::PropertySetGet *setget = (const ClassDB::PropertySetGet *)lua_touserdata(L, -1);
			if (setget && setget->_getptr) {
				MethodBind *mb = setget->_getptr;
				Variant::CallError err;
				Variant variant = mb->call(p_instance->owner, NULL, 0, err);
				if (err.error == Variant::CallError::CALL_OK) {
					l_push_variant(L, variant);
					return 1;
				} else {
					l_method_error(L, err);
				}
			}
			//2.if is method just return
			return 1;
		}
	}
	lua_pop(L, 1);

	//3.from luascirpt base get method
	Variant var;
	//get class
	l_push_luascript_ref(L, p_instance->script->lua_ref);
	//get function
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if (!lua_isnil(L, -1))
		return 1;
	return 0;
}
int LuaBindingHelper::meta_instance__newindex(lua_State *L) {
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	const char *index_name = l_get_key(L, 2);
	int idx = 3;
	int t = lua_type(L, idx);

	if (LUA_TNONE == t || LUA_TTHREAD == t || LUA_TLIGHTUSERDATA == t || LUA_TTABLE == t) {
		//p_instance->add_lua_property_type(index_name, t);
		lua_rawset(L, 1);
	} else if (LUA_TFUNCTION == t) {
		//p_script->add_lua_method(index_name);
		lua_rawset(L, 1);
	} else {
		Variant var;
		l_get_variant(L, idx, var);
		//set

		bool success = p_instance->set(index_name, var);
		if (!success) {
			luaL_error(L, "set %s.%s = %s error", String(Variant(p_instance->owner)).ascii().get_data(), String(index_name).ascii().get_data(), String(var).ascii().get_data());
		}
	}
	return 0;
}

int l_base_methodbind_wrapper(lua_State *L) {
	MethodBind *mb = (MethodBind *)lua_touserdata(L, lua_upvalueindex(1));
	Object *obj = (Object *)lua_touserdata(L, lua_upvalueindex(2));
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
		Variant &&ret = mb->call(obj, (const Variant **)args, top - 1, err);
		memdelete_arr(vars);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	} else {
		Variant &&ret = mb->call(obj, NULL, 0, err);
		if (Variant::CallError::CALL_OK == err.error) {
			l_push_variant(L, ret);
			return 1;
		}
	}
	l_method_error(L, err);
	return 0;
}

int meta_base_cls__index(lua_State *L) {
	StringName *class_name = (StringName *)lua_touserdata(L, lua_upvalueindex(1));

	const char *index_name = l_get_key(L, 2);
	MethodBind *mb = ClassDB::get_method(*class_name, index_name);
	if (mb != NULL) {
		lua_pushlightuserdata(L, mb);
		lua_pushcclosure(L, l_base_methodbind_wrapper, 1);
		return 1;
	}
	return 0;
}

void LuaBindingHelper::helper_push_instance(void *object) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;

	lua_newtable(L);
	//==base
	lua_newtable(L);
	if (p_instance->script->cls != NULL) {
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_pushlightuserdata(L, (void *)&p_instance->script->cls->name);
		//lua_pushlightuserdata(L, p_instance->owner);
		lua_pushcclosure(L, meta_base_cls__index, 1);
		lua_pushstring(L, "__index");
		lua_rawset(L, -3);
		lua_setmetatable(L, -2);
	} else {
		//TODO::Set BaseScriptFunction caller here
	}
	lua_setfield(L, -2, "base");

	//==push script default Variant
	for (auto E = p_instance->script->properties_default_value.front(); E != NULL; E = E->next()) {
		l_push_variant(L, E->value());
		lua_setfield(L, -2, String(E->key()).ascii().get_data());
	}

	//instance_pointer
	lua_pushlightuserdata(L, object);
	lua_setfield(L, -2, ".c_instance");

	lua_pushlightuserdata(L, (void *)&LuaBindingHelper::LUAINSTANCE);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_setmetatable(L, -2);
	l_ref_instance(L, object);
}

void LuaBindingHelper::l_ref_instance(lua_State *L, void *object) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
	lua_pushlightuserdata(L, (void *)&GD_INSTANCE_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	p_instance->lua_ref = luaL_ref(L, -2);
	lua_pop(L, 1);
}

void LuaBindingHelper::l_push_instance_ref(lua_State *L, int ref) {
	lua_pushlightuserdata(L, (void *)&GD_INSTANCE_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref);
	lua_remove(L, -2);
}

void LuaBindingHelper::l_unref_instance(void *object) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
	lua_pushlightuserdata(L, (void *)&GD_INSTANCE_REF);
	lua_rawget(L, LUA_REGISTRYINDEX);
	luaL_unref(L, -1, p_instance->lua_ref);
	lua_pop(L, 1);
}
bool LuaBindingHelper::l_instance_set(ScriptInstance *object, const StringName &p_name, const Variant &p_value) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
	l_push_instance_ref(L, p_instance->lua_ref);
	lua_pushstring(L, String(p_name).ascii().get_data());
	l_push_variant(L, p_value);
	lua_rawset(L, -3);
	lua_pop(L, 2);
	return true;
}
bool LuaBindingHelper::l_instance_get(const ScriptInstance *object, const StringName &p_name, Variant &r_ret) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
	l_push_instance_ref(L, p_instance->lua_ref);
	lua_pushstring(L, String(p_name).ascii().get_data());
	lua_rawget(L, -2);
	l_get_variant(L, -1, r_ret);
	lua_pop(L, 2);
	return true;
}

int LuaBindingHelper::meta_stringname__gc(lua_State *L) {
	void *ptr = lua_touserdata(L, 1);
	StringName *name = *((StringName **)ptr);
	delete name;
	return 0;
}
void LuaBindingHelper::l_push_stringname(lua_State *L, const char *name) {
	StringName **pp_stringName = (StringName **)lua_newuserdata(L, sizeof(StringName *));
	*pp_stringName = new StringName(name);
	lua_newtable(L);
	lua_pushcclosure(L, meta_stringname__gc, 0);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);
}
int LuaBindingHelper::pcall_callback_err_fun(lua_State *L) {
	lua_Debug debug = {};
	int ret = lua_getstack(L, 2, &debug); // 0是pcall_callback_err_fun自己, 1是error函数, 2是真正出错的函数.
	if (&debug != NULL) {
		// lua_getinfo(L, "Sln", &debug);
		// lua_pop(L, 1);
		// String msg = debug.short_src + String(":line ") + itos(debug.currentline) + "\n";
		// if (debug.name != 0) {
		// 	msg += String("(") + debug.namewhat + " " + debug.name + ")";
		// }
		// lua_pushstring(L, msg.utf8().get_data());
	}
	String errmsg = lua_tostring(L, -1);
	ERR_PRINT_ONCE(errmsg.utf8().get_data());

	return 1;
}

Variant LuaBindingHelper::instance_call(ScriptInstance *p_instance, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	LuaScriptInstance *p_si = (LuaScriptInstance *)p_instance;
#ifdef DEBUG_ENABLED
	//error
	lua_pushcfunction(L, pcall_callback_err_fun);
	int pos_err = lua_gettop(L);
#endif

	Variant var;
	//get class
	l_push_luascript_ref(L, p_si->script->lua_ref);
	//get function
	lua_pushstring(L, String(p_method).ascii().get_data());
	lua_rawget(L, -2);
	if (lua_isfunction(L, -1)) {
		//pushinstance luaref
		l_push_instance_ref(L, p_si->lua_ref);
		//args
		for (int i = 0; i < p_argcount; i++) {
			l_push_variant(L, *p_args[i]);
		}
		//5.3可以换成pcallk
#ifdef DEBUG_ENABLED
		if (lua_pcall(L, p_argcount + 1, 1, pos_err) == 0) {
			r_error.error = Variant::CallError::CALL_OK;
			l_get_variant(L, -1, var);
		} else {
			r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
		}
#else
		lua_call(L, p_argcount + 1, 1);
#endif

	} else {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	}
	r_error.argument = p_argcount;
	lua_settop(L, 0);
	return var;
}

int l_load(lua_State *L) {
	if (lua_isstring(L, 1)) {
		Variant var = ResourceLoader::load(lua_tostring(L, 1));
		l_push_variant(L, var);
	} else
		lua_pushnil(L);
	return 1;
}

void LuaBindingHelper::godotbind() {
	lua_getfield(L, LUA_GLOBALSINDEX, "GD");
	lua_pushcfunction(L, l_print);
	lua_setfield(L, -2, "print");

	//
	//basicfunctionbind
	lua_pushcfunction(L, l_load);
	lua_setfield(L, -2, "load");

	//pairs
	lua_pushcfunction(L, meta_variants__pairs);
	lua_setfield(L, -2, "pairs");
	lua_pop(L, 1);

	//TODO:: some basic engine func should bind in this
}

void LuaBindingHelper::register_class(lua_State *L, const ClassDB::ClassInfo *cls) {
	CharString s = String(cls->name).ascii();
	const char *typeName = s.get_data();
	//namespace GD
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

	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	{
		lua_pushlightuserdata(L, (void *)cls);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushlightuserdata(L, (void *)cls);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);
			//method
			const StringName *key = cls->method_map.next(NULL);
			while (key) {
				MethodBind *mb = cls->method_map.get(*key);
				lua_pushlightuserdata(L, (void *)mb);
				lua_pushcclosure(L, l_methodbind_wrapper, 1);
				lua_setfield(L, -2, String(*key).ascii().get_data());
				key = cls->method_map.next(key);
			}
			//propterty
			const StringName *prop = cls->property_setget.next(NULL);
			while (prop) {
				const ClassDB::PropertySetGet *setget = cls->property_setget.getptr(*prop);
				lua_pushlightuserdata(L, (void *)setget);
				lua_setfield(L, -2, String(*prop).ascii().get_data());
				prop = cls->property_setget.next(prop);
			}

			//constant
			const StringName *constant = cls->constant_map.next(NULL);
			while (constant) {
				const int value = cls->constant_map.get(*constant);
				lua_pushinteger(L, value);
				lua_setfield(L, -2, String(*constant).ascii().get_data());
				constant = cls->constant_map.next(constant);
			}

			//enum
			const StringName *m_enum = cls->enum_map.next(NULL);
			while (m_enum) {
				const List<StringName> *m_enums = cls->enum_map.getptr(*m_enum);
				lua_newtable(L);
				{
					for (int i = 0; i < m_enums->size(); i++) {
						lua_pushinteger(L, i);
						lua_setfield(L, -2, String((*m_enums)[i]).ascii().get_data());
					}
				}
				lua_setfield(L, -2, String(*m_enum).ascii().get_data());
				m_enum = cls->enum_map.next(m_enum);
			}
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}

void LuaBindingHelper::regitser_builtins(lua_State *L) {
	LuaBuiltin::regitser_builtins(L);
}

void LuaBindingHelper::initialize() {
	L = luaL_newstate();

	luaL_openlibs(L);
	lua_settop(L, 0);

	//GD namespace
	lua_newtable(L);
	lua_setfield(L, LUA_GLOBALSINDEX, "GD");

	//GD class
	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//godot function
	godotbind();

	//hidden script space
	lua_pushlightuserdata(L, (void *)&GD_SCRIPT_REF);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//hidden instance space
	lua_pushlightuserdata(L, (void *)&GD_INSTANCE_REF);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//hidden object space
	lua_pushlightuserdata(L, (void *)&WEAK_UBOX);
	lua_newtable(L);
	/* make weak value metatable for ubox table to allow userdata to be
	   garbage-collected */
	lua_newtable(L);
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3); /* stack: string ubox mt */
	lua_setmetatable(L, -2); /* stack: string ubox */
	lua_rawset(L, LUA_REGISTRYINDEX);

	//ref ubox
	lua_pushlightuserdata(L, (void *)&REF_UBOX);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//Object binding
	lua_pushlightuserdata(L, (void *)&LUAOBJECT);
	lua_newtable(L);
	{
		luaL_Reg meta_methods[] = {
			{ "__gc", meta_object__gc },
			{ "__index", meta_object__index },
			{ "__newindex", meta_object__newindex },
			{ "__tostring", meta_object__tostring },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}
	lua_rawset(L, LUA_REGISTRYINDEX);

	//Variant binding
	lua_pushlightuserdata(L, (void *)&LUAVARIANT);
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
			lua_pushcclosure(L, meta_variants__evaluate, 1);
			lua_rawset(L, -3);
		}

		static luaL_Reg meta_methods[] = {
			{ "__gc", meta_variants__gc },
			{ "__index", meta_variants__index },
			{ "__newindex", meta_variants__newindex },
			{ "__tostring", meta_variants__tostring },
			// { "__pairs", meta_bultins__pairs },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}
	lua_rawset(L, LUA_REGISTRYINDEX);

	//LuaScript binding
	lua_pushlightuserdata(L, (void *)&LUASCRIPT);
	lua_newtable(L);
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
	lua_rawset(L, LUA_REGISTRYINDEX);

	//LuaInstance binding
	lua_pushlightuserdata(L, (void *)&LUAINSTANCE);
	lua_newtable(L);
	{
		static luaL_Reg meta_methods[] = {
			{ "__gc", meta_instance__gc },
			{ "__index", meta_instance__index },
			{ "__newindex", meta_instance__newindex },
			{ "__tostring", meta_instance__tostring },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, meta_methods, 0);
	}
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

	{
		const StringName *key = ClassDB::classes.next(NULL);
		while (key) {
			const ClassDB::ClassInfo *cls = ClassDB::classes.getptr(*key);
			link__index_class(L, cls);
			key = ClassDB::classes.next(key);
		}
	}
	//regise builtin
	regitser_builtins(L);
}
void LuaBindingHelper::link__index_class(lua_State *L, const ClassDB::ClassInfo *cls) {
	lua_pushlightuserdata(L, (void *)&GD_CLASS);
	lua_rawget(L, LUA_REGISTRYINDEX);
	{
		const ClassDB::ClassInfo *top = cls;
		while (top) {
			if (top->inherits_ptr == NULL) {
				break;
			}
			lua_pushlightuserdata(L, (void *)top);
			lua_rawget(L, -2);
			if (lua_getmetatable(L, -1) == 0) {
				lua_newtable(L);
				lua_pushlightuserdata(L, (void *)top->inherits_ptr);
				lua_rawget(L, -4);
				lua_setfield(L, -2, "__index");
				lua_setmetatable(L, -2);
			} else {
				lua_pop(L, 2);
				break;
			}
			lua_pop(L, 1);
			top = top->inherits_ptr;
		}
	}
	lua_pop(L, 1);
}
void LuaBindingHelper::uninitialize() {
	lua_close(L);
	L = NULL;
}

Error LuaBindingHelper::script(const String &p_source) {
	ERR_FAIL_NULL_V(L, ERR_DOES_NOT_EXIST);
	luaL_loadstring(L, p_source.utf8());
	Error err = luacall();
	return err;
}

Error LuaBindingHelper::script(void *p_script, const String &p_source) {
	ERR_FAIL_NULL_V(L, ERR_DOES_NOT_EXIST);
	bind_script_function("extends", p_script, LuaBindingHelper::l_extends);
	luaL_loadstring(L, p_source.utf8());
	Error err = luacall();
	unbind_script_function("extends");
	return err;
}

Error LuaBindingHelper::bytecode(void *p_script, const Vector<uint8_t> &p_bytecode) {
	ERR_FAIL_NULL_V(L, ERR_DOES_NOT_EXIST);
	bind_script_function("extends", p_script, LuaBindingHelper::l_extends);
	luaL_loadbufferx(L, (const char *)p_bytecode.ptr(), p_bytecode.size(), "", "b");
	Error err = luacall();
	unbind_script_function("extends");
	return err;
}

Error LuaBindingHelper::luacall() {
#ifdef DEBUG_ENABLED
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
#else
	lua_pcall(L, 0, LUA_MULTRET);
#endif

	return OK;
}