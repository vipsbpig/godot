#ifndef LUABINDINGHELPER_H
#define LUABINDINGHELPER_H

#include "core/object.h"
#include "lua.hpp"

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM == 501
/*
** Adapted from Lua 5.2.0
*/
static void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup) {
	luaL_checkstack(L, nup + 1, "too many upvalues");
	for (; l->name != NULL; l++) { /* fill the table with given functions */
		int i;
		lua_pushstring(L, l->name);
		for (i = 0; i < nup; i++) /* copy upvalues to the top */
			lua_pushvalue(L, -(nup + 1));
		lua_pushcclosure(L, l->func, nup); /* closure with those upvalues */
		lua_settable(L, -(nup + 3));
	}
	lua_pop(L, nup); /* remove upvalues */
}

static int lua_isinteger(lua_State *L, int index) {
	if (lua_type(L, index) == LUA_TNUMBER) {
		lua_Number n = lua_tonumber(L, index);
		lua_Integer i = lua_tointeger(L, index);
		if (i == n)
			return 1;
	}
	return 0;
}

#endif

//push a Variant into lua stack
void l_push_variant(lua_State *L, const Variant &var);
void l_push_bulltins_type(lua_State *L, const Variant &var);
void l_get_variant(lua_State *L, int idx, Variant &var);

class LuaBindingHelper {
	lua_State *L;

public:
	static int l_print(lua_State *L);

	//===lua methods
	static int l_extends(lua_State *L);

	void bind_script_function(const char *name, void *p_script, lua_CFunction fn);
	void unbind_script_function(const char *name);

private:
	//===native binding
	static int create_user_data(lua_State *L);

	//===LuaObject lua meta methods

	static int meta__gc(lua_State *L);
	static int meta__tostring(lua_State *L);
	static int meta__index(lua_State *L);
	static int meta__newindex(lua_State *L);
	static int l_methodbind_wrapper(lua_State *L);
	//===lua variant convert helper

private:
	//===LuaVariant lua meta methods

	static int meta_bultins__evaluate(lua_State *L);
	static int meta_bultins__gc(lua_State *L);
	static int meta_bultins__tostring(lua_State *L);
	static int meta_bultins__index(lua_State *L);
	static int meta_bultins__newindex(lua_State *L);
	static int meta_bultins__pairs(lua_State *L);
	static int l_bultins_caller_wrapper(lua_State *L);

	//TO BE CONFIRM
	static int l_builtins_iterator(lua_State *L);

	//===LuaScript lua meta methods

	static int meta_script__gc(lua_State *L);
	static int meta_script__tostring(lua_State *L);
	static int meta_script__index(lua_State *L);
	static int meta_script__newindex(lua_State *L);
	void l_push_luascript_ref(lua_State *L, int ref);
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
	//void l_push_instance_ref(lua_State *L, int ref);
	static void l_ref_instance(lua_State *L, void *object);

public:
	void l_unref_instance(void *object);

private:
	static int pcall_callback_err_fun(lua_State *L);

public:
	Variant instance_call(ScriptInstance *p_instance, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);

private:
	//===global

	void openLibs(lua_State *L);
	void globalbind();
	void register_class(lua_State *L, const ClassDB::ClassInfo *cls);
	void regitser_builtins(lua_State *L);

public:
	LuaBindingHelper();

	void initialize();
	void uninitialize();

	Error script(const String &p_source);

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
