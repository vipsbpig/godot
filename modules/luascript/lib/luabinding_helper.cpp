#include "luabinding_helper.h"
#include "../debug.h"
#include "../luascript.h"
#include "../luascript_instance.h"
#include "luabuiltin.h"
#include "scene/main/node.h"
#include <lualib.h>

static Variant *luaL_checkvariant(lua_State *L, int idx) {
	//void *ptr = luaL_checkudata(L, idx, "LuaVariant");
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
			luaL_getmetatable(L, "LuaInstance");
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
void l_push_vector2_type(lua_State *L, const Vector2 &var) {
	lua_newtable(L);
	lua_pushnumber(L, var.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, var.y);
	lua_setfield(L, -2, "y");
	lua_pushstring(L, "vector2");
	lua_setfield(L, -2, "VT");
}
// void l_push_point2_type(lua_State *L, const Point2 &var) {
// 	l_push_vector2_type(L, var);
// 	lua_pushstring(L, "point2");
// 	lua_setfield(L, -2, "VT");
// }
// void l_push_size2_type(lua_State *L, const Size2 &var) {
// 	l_push_vector2_type(L, var);
// 	lua_pushstring(L, "size2");
// 	lua_setfield(L, -2, "VT");
// }
void l_push_rect2_type(lua_State *L, const Rect2 &var) {
	lua_newtable(L);
	l_push_vector2_type(L, var.position);
	lua_setfield(L, -2, "position");
	l_push_vector2_type(L, var.size);
	lua_setfield(L, -2, "size");
	lua_pushstring(L, "rect2");
	lua_setfield(L, -2, "VT");
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
				//printf("push ref:%s count:%d\n", String(Variant(ref)).ascii().get_data(), ref->reference_get_count());
				ref->reference();
			}
		} break;
		case Variant::VECTOR2: {
			l_push_vector2_type(L, (Vector2)var);
			break;
		}
		case Variant::RECT2: {
			l_push_rect2_type(L, (Rect2)var);
			break;
		}
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
		case Variant::DICTIONARY: {
			l_push_bulltins_type(L, var);
		} break;
		case Variant::ARRAY: {
			l_push_array_type(L, var);
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

void l_push_array_type(lua_State *L, const Variant &var) {
	lua_newtable(L);
	int idx = 0;
	bool r_valid = false;
	do {
		Variant value = var.get(idx, &r_valid);
		if (!r_valid) break;
		lua_pushinteger(L, idx);
		l_push_variant(L, value);
		lua_rawset(L, -3);
		idx++;
	} while (r_valid);
	luaL_getmetatable(L, "LuaArray");
	lua_setmetatable(L, -2);
}

void l_push_bulltins_type(lua_State *L, const Variant &var) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("builtIn Type:%s", Variant::get_type_name(var.get_type()).ascii().get_data());
#endif
	Variant **ptr = (Variant **)lua_newuserdata(L, sizeof(Variant *));
	*ptr = memnew(Variant);
	**ptr = var;
	luaL_getmetatable(L, "LuaVariant");
	lua_setmetatable(L, -2);
}

Vector2 l_get_vector2(lua_State *L, int idx) {
	lua_getfield(L, idx, "x");
	auto x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "y");
	auto y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return Vector2(x, y);
}
Rect2 l_get_rect2(lua_State *L, int idx) {
	lua_getfield(L, idx, "position");
	auto x = l_get_vector2(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, idx, "size");
	auto y = l_get_vector2(L, -1);
	lua_pop(L, 1);
	return Rect2(x, y);
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
			lua_pushliteral(L, "VT");
			lua_rawget(L, idx);
			if (lua_isstring(L, -1)) {
				const char *vt = lua_tostring(L, -1);
				if (strncmp(vt, "vector2", 7) == 0) {
					var = l_get_vector2(L, idx);
				} else if (strncmp(vt, "rect2", 5) == 0) {
					var = l_get_rect2(L, idx);
				}
				return;
			}
			lua_pop(L, 1);
			if (lua_getmetatable(L, 1)) {
				luaL_getmetatable(L, "LuaArray");
				if (lua_rawequal(L, -1, -2)) {
					Array arr;
					if (idx < 0) {
						idx = lua_gettop(L) + idx + 1;
					}
					/* table 放在索引 't' 处 */
					lua_pushnil(L); /* 第一个 key */
					while (lua_next(L, idx) != 0) {
						/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
						Variant tmp;
						l_get_variant(L, -2, tmp);
						arr.push_back(tmp);
						/* 移除 'value' ；保留 'key' 做下一次迭代 */
						lua_pop(L, 1);
					}
					lua_pop(L, 1);
					return;
				}
				lua_pop(L, 1);

				luaL_getmetatable(L, "LuaInstance");
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
					//lua_getfield(L, LUA_REGISTRYINDEX, "LuaObject");
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
							//printf("Ref %s\n",String(var).ascii().get_data());

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

const char *l_get_key(lua_State *L, int idx) {
	return lua_tostring(L, idx);
}

const char LuaBindingHelper::LUAOBJECT = 0;
const char LuaBindingHelper::LUAVARIANT = 0;
const char LuaBindingHelper::LUASCRIPT = 0;
const char LuaBindingHelper::LUAINSTANCE = 0;

const char LuaBindingHelper::GDCLASS = 0;
const char LuaBindingHelper::WEAK_UBOX = 0;
const char LuaBindingHelper::REF_UBOX = 0;
const char LuaBindingHelper::LUA_SCRIPT_REF = 0;
const char LuaBindingHelper::LUA_INSTANCE_REF = 0;

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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
		const char *base = String(cls->name).ascii().get_data();
		print_format("l_extends from:%s %d script:%d", base, cls, p_script);
#endif
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

#ifdef LUA_SCRIPT_DEBUG_ENABLED
		print_line("Should extends from a res.Now Only extends from Object");
#endif
		//TODO::如果是字符串就从路径加载脚本去继承来写
		//现在临时用Object来代替
		//
		const ClassDB::ClassInfo *cls = &ClassDB::classes["Object"];
#ifdef LUA_SCRIPT_DEBUG_ENABLED
		const char *base = String(cls->name).ascii().get_data();
		print_format("l_extends from:%s", base);
#endif
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
	Object *object = cls->creation_func();
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("Name %s call new object_ptr:%d ", object->get_class().ascii().get_data(), object);
#endif
	script_pushobject(L, object);
	return 1;
}
int LuaBindingHelper::script_pushobject(lua_State *L, Object *object) {
	Object **ud;
	lua_pushstring(L, "weak_ubox");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushnumber(L, object->get_instance_id());
	lua_rawget(L, -2);
	if (lua_type(L, -1) == LUA_TUSERDATA) {
		ud = (Object **)lua_touserdata(L, -1);
		if (*ud == object) {
			lua_replace(L, -2);
			return 1;
		}
		// C 对象指针被释放后，有可能地址被重用。
		// 这个时候，可能取到曾经保存起来的 userdata ，里面的指针必然为空。
		//ERR_FAIL_COND_V(*ud != NULL, -1);
	}
	ud = (Object **)lua_newuserdata(L, sizeof(Object *));
	*ud = object;
	///----object---
	//lua_newtable(L);
	//props
	// {
	// 	lua_newtable(L);

	// 	const ClassDB::ClassInfo *top = ClassDB::classes.getptr(object->get_class_name());
	// 	while (top) {
	// 		const List<PropertyInfo> *props_list = &top->property_list;
	// 		for (auto E = props_list->front(); E != props_list->back(); E = E->next()) {
	// 			const ClassDB::PropertySetGet *psg = top->property_setget.getptr(E->get().name);
	// 			if (psg) {
	// 				lua_pushlightuserdata(L, (void *)psg);
	// 				lua_setfield(L, -2, E->get().name.ascii().get_data());
	// 			}

	// 			const int *c = top->constant_map.getptr(E->get().name);
	// 			if (c) {
	// 				lua_pushinteger(L, *c);
	// 				lua_setfield(L, -2, E->get().name.ascii().get_data());
	// 			}
	// 		}
	// 		top = top->inherits_ptr;
	// 	}

	// 	lua_setfield(L, -2, "props");
	// }
	//luaObject
	// lua_pushboolean(L, 1);
	// lua_setfield(L, -2, "LuaObject");

	// //meta func
	// {
	// 	luaL_Reg meta_methods[] = {
	// 		{ "__gc", meta_object__gc },
	// 		{ "__index", meta_object__index },
	// 		{ "__newindex", meta_object__newindex },
	// 		{ "__tostring", meta_object__tostring },
	// 		{ NULL, NULL },
	// 	};
	// 	luaL_setfuncs(L, meta_methods, 0);
	// }
	lua_pushlightuserdata(L, (void *)&LUAOBJECT);
	lua_rawget(L, LUA_REGISTRYINDEX);
	//lua_getfield(L, LUA_REGISTRYINDEX, "LuaObject");
	lua_setmetatable(L, -2);
	///---------
	lua_pushnumber(L, object->get_instance_id());
	lua_pushvalue(L, -2);
	lua_rawset(L, -5);
	lua_replace(L, -3);
	lua_pop(L, 1);
	return 1;
}

void LuaBindingHelper::push_strong_ref(lua_State *L, Object *object) {
	script_pushobject(L, object);
	lua_pushstring(L, "ref_ubox");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushnumber(L, object->get_instance_id());
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 2);
};
void LuaBindingHelper::del_strong_ref(lua_State *L, Object *object) {
	lua_pushstring(L, "ref_ubox");
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
		//printf("__gc ref:%s count:%d\n", String(Variant(ref)).ascii().get_data(), ref->reference_get_count());
		if (ref->unreference()) {
			//printf("memdelete\n");
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta__index: %s call %s", obj->get_class().ascii().get_data(), String(index_name).ascii().get_data());
#endif
	lua_getfield(L, LUA_REGISTRYINDEX, "gd");
	{

		lua_getfield(L, -1, obj->get_class().ascii().get_data());
		lua_getfield(L, -1, index_name);

		if (!lua_isnil(L, -1)) {
			//1.如果拥有变量，压入
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

			//2.如果是方法，压入
			return 1;
		}
	}
	lua_pop(L, 1);

	//3.free方法
	if (strncmp(index_name, "free", 4) == 0) {
		lua_pushcclosure(L, l_object_free, 0);
		return 1;
	}
	return 0;
}
int LuaBindingHelper::meta_object__newindex(lua_State *L) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("call %s", "meta__newindex");
#endif
	Object **ud = (Object **)lua_touserdata(L, 1);
	Object *obj = *ud;
	if (obj == NULL) {
		const char *index_name = lua_tostring(L, 2);
		luaL_error(L, "Failed To Set Field :'%s' To NULL Object", index_name);
		return 0;
	}

	// bool valid = false;
	// obj->set(l_get_key(L, 2), value, &valid);
	// if (!valid)
	// 	luaL_error(L, "Unable to set field: '%s'", lua_tostring(L, 2));

	lua_getfield(L, LUA_REGISTRYINDEX, "gd");
	{

		lua_getfield(L, -1, obj->get_class().ascii().get_data());
		lua_getfield(L, -1, l_get_key(L, 2));

		if (!lua_isnil(L, -1)) {
			//1.如果拥有变量，压入
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("var:%d gc", var);
#endif
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
	//方法
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
	lua_pushcclosure(L, l_bultins_caller_wrapper, 1);

	return 1;
}
int LuaBindingHelper::meta_bultins__newindex(lua_State *L) {
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
			luaL_error(L, "Cannot pairs an %s value", var.get_type_name(var.get_type()).ascii().get_data());
			break;
	}
	return 0;
}

int LuaBindingHelper::l_bultins_caller_wrapper(lua_State *L) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("l_bultins_caller_wrapper");
#endif
	//const char *key = luaL_checkstring(L, lua_upvalueindex(1));
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta_script__gc nothing to do");
#endif
	return 0;
}

int LuaBindingHelper::meta_script__tostring(lua_State *L) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta_script__tostring.");
#endif
	LuaScript *p_script = luaL_getscript(L, 1);
	auto var = p_script->get_instance_base_type();
	l_push_variant(L, var);
	return 1;
}

int LuaBindingHelper::meta_script__index(lua_State *L) {
	LuaScript *p_script = luaL_getscript(L, 1);
	const char *index_name = l_get_key(L, 2);
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	const char *base = String(p_script->cls->name).ascii().get_data();
	print_format("meta_script__index:%s base:%s %d script:%d", lua_tostring(L, 2), base, p_script->cls, p_script);
#endif
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	const char *base = String(p_script->cls->name).ascii().get_data();
	print_format("meta_script__newindex:%slua_typ:%d  base:%s script:%d lua_t:%d", lua_tostring(L, 2), t, base, p_script->cls, p_script);
#endif
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
	lua_pushstring(L, "lua_script_ref");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	p_script->lua_ref = luaL_ref(L, -2);
	lua_pop(L, 1);
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("l_ref_luascript:%d s:%d", p_script->lua_ref, p_script);
#endif
}

void LuaBindingHelper::l_push_luascript_ref(lua_State *L, int ref) {
	lua_pushstring(L, "lua_script_ref");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref);
	lua_remove(L, -2);
}

void LuaBindingHelper::l_unref_luascript(void *object) {
	LuaScript *p_script = (LuaScript *)object;
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	printf("l_unref_luascript:%d s:%d\n", p_script->lua_ref, p_script);
#endif

	lua_pushstring(L, "lua_script_ref");
	lua_rawget(L, LUA_REGISTRYINDEX);
	luaL_unref(L, -1, p_script->lua_ref);
	lua_pop(L, 1);
}

int LuaBindingHelper::meta_instance__gc(lua_State *L) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta_instance__gc nothing to do");
#endif
	return 0;
}
int LuaBindingHelper::meta_instance__tostring(lua_State *L) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta_instance__tostring");
#endif
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	lua_pushstring(L, String(Variant(p_instance->get_owner())).ascii().get_data());
	return 1;
}
int LuaBindingHelper::meta_instance__index(lua_State *L) {
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	const char *index_name = l_get_key(L, 2);
	//print_format("meta_instance__index:%s instance:%s", lua_tostring(L, 2), String(Variant(p_instance->get_owner())).ascii().get_data());
	//1.如果是变量，压入
	// bool success = false;
	// Variant variant;
	// success = p_instance->get(index_name, variant);
	// if (success) {
	// 	l_push_variant(L, variant);
	// 	return 1;
	// }
	lua_getfield(L, LUA_REGISTRYINDEX, "gd");
	{
		lua_getfield(L, -1, p_instance->owner->get_class().ascii().get_data());
		lua_getfield(L, -1, index_name);
		if (!lua_isnil(L, -1)) {

			//1.如果拥有变量，压入
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
			//2.如果是方法，压入

			return 1;
		}
	}
	lua_pop(L, 1);

	// MethodBind *mb = ClassDB::get_method(p_instance->owner->get_class_name(), index_name);
	// if (mb != NULL) {
	// 	lua_pushlightuserdata(L, mb);
	// 	lua_pushcclosure(L, l_methodbind_wrapper, 1);
	// 	return 1;
	// }

	//3.类方法获取
	Variant var;
	//get class
	l_push_luascript_ref(L, p_instance->script->lua_ref);
	//get function
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if (!lua_isnil(L, -1))
		return 1;
	return 0;
	// //4.普通获取--因为inde是在原本没有才会调用的
	// lua_pop(L, 2); //pop script and result
	// lua_rawget(L, -2);
	// return 1;
}
int LuaBindingHelper::meta_instance__newindex(lua_State *L) {
	LuaScriptInstance *p_instance = luaL_getinstance(L, 1);
	const char *index_name = l_get_key(L, 2);
	int idx = 3;
	int t = lua_type(L, idx);
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("meta_instance__newindex:%s lua_typ:%s ", lua_tostring(L, 2), lua_typename(L, t));
#endif
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
	//压入方法来调用
	//
	StringName *class_name = (StringName *)lua_touserdata(L, lua_upvalueindex(1));
	//Object *pushobj = (Object *)lua_touserdata(L, lua_upvalueindex(2));

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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("helper_push_instance:%d s:%s", p_instance->lua_ref, String(Variant(p_instance->get_owner())).ascii().get_data());
#endif
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

	luaL_getmetatable(L, "LuaInstance");
	lua_setmetatable(L, -2);
	l_ref_instance(L, object);
}

void LuaBindingHelper::l_ref_instance(lua_State *L, void *object) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
	lua_pushstring(L, "lua_instance_ref");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	p_instance->lua_ref = luaL_ref(L, -2);
	lua_pop(L, 1);
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("l_ref_instance:%d s:%s", p_instance->lua_ref, String(Variant(p_instance->get_owner())).ascii().get_data());
#endif
}

void LuaBindingHelper::l_push_instance_ref(lua_State *L, int ref) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("l_push_instance_ref :%d ", ref);
#endif
	lua_pushstring(L, "lua_instance_ref");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, -1, ref);
	lua_remove(L, -2);
}

void LuaBindingHelper::l_unref_instance(void *object) {
	LuaScriptInstance *p_instance = (LuaScriptInstance *)object;
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("l_unref_instance:%d s:%s", p_instance->lua_ref, String(Variant(p_instance->get_owner())).ascii().get_data());
#endif
	lua_pushstring(L, "lua_instance_ref");
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_format("instance_call: script:%d argc:%d", p_si->script.ptr(), p_argcount);
#endif

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
		//pcall
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
#ifdef LUA_SCRIPT_DEBUG_ENABLED
		print_format("script cannot get function:%s", String(p_method).ascii().get_data());
#endif
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
		//{ LUA_MATHLIBNAME, luaopen_math },
		{ LUA_DBLIBNAME, luaopen_debug },
		//{"lua-utf8", luaopen_utf8},
		{ NULL, NULL }
	};

	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		luaL_register(L, lib->name, lib);
	}
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
	lua_pushcfunction(L, meta_bultins__pairs);
	lua_setfield(L, -2, "pairs");
	lua_pop(L, 1);
}

void LuaBindingHelper::register_class(lua_State *L, const ClassDB::ClassInfo *cls) {
	// if (!(String(cls->name) == "Object" || String(cls->name) == "Node" || String(cls->name) == "_OS" || String(cls->name) == "Node2D"))
	// 	return;
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	printf("regist:[%s:%s]\n", String(cls->name).ascii().get_data(), String(cls->inherits).ascii().get_data());
#endif

	CharString s = String(cls->name).ascii();
	const char *typeName = s.get_data();
	//原生调用
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

	//需要遍历
	//
	lua_getfield(L, LUA_REGISTRYINDEX, "gd");
	{
		if (String(cls->name) == "CanvasItem") {
			printf("regist:[%s:%s]\n", String(cls->name).ascii().get_data(), String(cls->inherits).ascii().get_data());
		}
		//
		//const char *typeName = String(cls->name).ascii().get_data();
		lua_getfield(L, -1, String(cls->name).ascii().get_data());
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, -3, String(cls->name).ascii().get_data());

			const StringName *key = cls->method_map.next(NULL);
			while (key) {
				MethodBind *mb = cls->method_map.get(*key);
				lua_pushlightuserdata(L, (void *)mb);
				lua_pushcclosure(L, l_methodbind_wrapper, 1);
				lua_setfield(L, -2, String(*key).ascii().get_data());
				key = cls->method_map.next(key);
			}

			const StringName *prop = cls->property_setget.next(NULL);
			while (prop) {
				const ClassDB::PropertySetGet *setget = cls->property_setget.getptr(*prop);
				lua_pushlightuserdata(L, (void *)setget);
				lua_setfield(L, -2, String(*prop).ascii().get_data());
				prop = cls->property_setget.next(prop);
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
	// luaopen_base(L);
	// luaopen_table(L);
	// luaopen_math(L);
	// luaopen_debug(L);
	// luaopen_jit(L);
	luaL_openlibs(L);
	lua_settop(L, 0);

	//GD namespace
	lua_newtable(L);
	lua_setfield(L, LUA_GLOBALSINDEX, "GD");

	//GD namespace
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "gd");

	//godot function
	godotbind();

	//hidden script space
	lua_pushstring(L, "lua_script_ref");
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//hidden instance space
	lua_pushstring(L, "lua_instance_ref");
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//hidden object space
	lua_pushstring(L, "weak_ubox");
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
	lua_pushstring(L, "ref_ubox");
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	//Object binding
	//luaL_newmetatable(L, "LuaObject");
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
			// { "__pairs", meta_bultins__pairs },
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

	//LuaInstance binding
	luaL_newmetatable(L, "LuaInstance");
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
	lua_pop(L, 1);

	//LuaArray binding
	luaL_newmetatable(L, "LuaArray");
	{
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
	lua_getfield(L, LUA_REGISTRYINDEX, "gd");
	{
		const ClassDB::ClassInfo *top = cls;
		while (top) {
			const char *topname = String(top->name).ascii().get_data();
			if (top->inherits_ptr == NULL) {
				break;
			}
			lua_getfield(L, -1, topname);
			if (lua_getmetatable(L, -1) == 0) {
				lua_newtable(L);
				lua_getfield(L, -3, String(top->inherits_ptr->name).ascii().get_data());
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

Error LuaBindingHelper::bytecode(const Vector<uint8_t> &p_bytecode) {
	luaL_loadbufferx(L, (const char *)p_bytecode.ptr(), p_bytecode.size(), "", "b");
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
