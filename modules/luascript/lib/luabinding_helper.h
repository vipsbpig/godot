#ifndef LUABINDINGHELPER_H
#define LUABINDINGHELPER_H

#include "core/object.h"
#include "lua.hpp"

//push a Variant into lua stack
void l_push_variant(lua_State *L, const Variant &var);
void l_push_bulltins_type(lua_State *L, const Variant &var);
void l_get_variant(lua_State *L, int idx, Variant &var);

class LuaBindingHelper {

	lua_State *L;

public:
	static const char LUAOBJECT;
	static const char LUAVARIANT;
	static const char LUASCRIPT;
	static const char LUAINSTANCE;
	//=====
	static const char GD_CLASS;
	static const char WEAK_UBOX;
	static const char REF_UBOX;
	static const char GD_SCRIPT_REF;
	static const char GD_INSTANCE_REF;

	static int l_print(lua_State *L);

private:
	//===lua methods
	static int l_extends(lua_State *L);

	void bind_script_function(const char *name, void *p_script, lua_CFunction fn);
	void unbind_script_function(const char *name);

	//===native binding
	static int create_user_data(lua_State *L);

public:
	static int script_pushobject(lua_State *L, Object *object);
	static void push_strong_ref(lua_State *L, Object *object);
	static void del_strong_ref(lua_State *L, Object *Object);

private:
	//===LuaObject lua meta methods
	static int meta_object__gc(lua_State *L);
	static int meta_object__tostring(lua_State *L);
	static int meta_object__index(lua_State *L);
	static int meta_object__newindex(lua_State *L);
	static int l_object_free(lua_State *L);

	//==Reference add and remove by engine
public:
	void l_add_reference(Object *p_reference);
	bool l_del_reference(Object *p_reference);

	//===lua variant convert helper

private:
	//===LuaVariant lua meta methods

	static int meta_variants__evaluate(lua_State *L);
	static int meta_variants__gc(lua_State *L);
	static int meta_variants__tostring(lua_State *L);
	static int meta_variants__index(lua_State *L);
	static int meta_variants__newindex(lua_State *L);
	static int meta_variants__pairs(lua_State *L);
	static int l_variants_caller_wrapper(lua_State *L);

	//TO BE CONFIRM
	static int l_variants_iterator(lua_State *L);

	//===LuaScript lua meta methods

	static int meta_script__gc(lua_State *L);
	static int meta_script__tostring(lua_State *L);
	static int meta_script__index(lua_State *L);
	static int meta_script__newindex(lua_State *L);
	static void l_push_luascript_ref(lua_State *L, int ref);
	static void l_ref_luascript(lua_State *L, void *object);

public:
	void l_unref_luascript(void *object);

private:
	//===LuaInstance lua code
	//===LuaInstance lua meta methods

	static int meta_instance__gc(lua_State *L);
	static int meta_instance__tostring(lua_State *L);
	static int meta_instance__index(lua_State *L);
	static int meta_instance__newindex(lua_State *L);
	void l_push_instance_ref(lua_State *L, int ref);
	static void l_ref_instance(lua_State *L, void *object);

public:
	void helper_push_instance(void *object);
	void l_unref_instance(void *object);
	bool l_instance_set(ScriptInstance *p_instance, const StringName &p_name, const Variant &p_value);
	bool l_instance_get(const ScriptInstance *p_instance, const StringName &p_name, Variant &r_ret);

private:
	//stringNameCache
	static int meta_stringname__gc(lua_State *L);
	static void l_push_stringname(lua_State *L, const char *name);

private:
	static int pcall_callback_err_fun(lua_State *L);

public:
	Variant instance_call(ScriptInstance *p_instance, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);

private:
	//===global

	void openLibs(lua_State *L);
	void godotbind();
	void register_class(lua_State *L, const ClassDB::ClassInfo *cls);
	void link__index_class(lua_State *L, const ClassDB::ClassInfo *cls);

	void regitser_builtins(lua_State *L);

public:
	LuaBindingHelper();

	void initialize();
	void uninitialize();

	Error script(const String &p_source);
	Error script(void *p_script, const String &p_source);
	Error bytecode(void *p_script, const Vector<uint8_t> &p_source);
	Error luacall();

public:
	static void stackDump(lua_State *L) {
		int i = lua_gettop(L);
		printf(" ----------------  Stack Dump ----------------\n");
		while (i) {
			int t = lua_type(L, i);
			switch (t) {
				case LUA_TSTRING:
					printf("%d:`%s'\n", i, lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					printf("%d: %s\n", i, lua_toboolean(L, i) ? "true" : "false");
					break;
				case LUA_TNUMBER:
					printf("%d: %g\n", i, lua_tonumber(L, i));
					break;
				default: printf("%d: %s\n", i, lua_typename(L, t)); break;
			}
			i--;
		}
		printf("--------------- Stack Dump Finished ---------------\n");
	}
};

#endif // LUABINDINGHELPER_H
