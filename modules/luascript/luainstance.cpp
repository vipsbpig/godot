#include "luascript.h"
#include "debug.h"
#include "solhelper.hpp"

//=================LUAScriptInstance==========

LuaScriptInstance::LuaScriptInstance() {
}

static Object *luaL_checkobject(lua_State *L, int idx) {

	size_t id = *(size_t *)luaL_checkudata(L, idx, "LuaObject");
	return ObjectDB::get_instance(id);
}

LuaScriptInstance::~LuaScriptInstance() {
}

bool LuaScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	return false;
}

bool LuaScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void LuaScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
}

Variant::Type LuaScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	return Variant::Type();
}

Object *LuaScriptInstance::get_owner() {
	return nullptr;
}

void LuaScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
}

bool LuaScriptInstance::has_method(const StringName &p_method) const {
	return false;
}

Variant LuaScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	return Variant();
}

void LuaScriptInstance::call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {
}

void LuaScriptInstance::call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount) {
}

void LuaScriptInstance::notification(int p_notification) {
}

void LuaScriptInstance::refcount_incremented() {
}

bool LuaScriptInstance::refcount_decremented() {
	return false;
}

Ref<Script> LuaScriptInstance::get_script() const {
	return Ref<Script>();
}

MultiplayerAPI::RPCMode LuaScriptInstance::get_rpc_mode(const StringName &p_method) const {
	return MultiplayerAPI::RPCMode();
}

MultiplayerAPI::RPCMode LuaScriptInstance::get_rset_mode(const StringName &p_variable) const {
	return MultiplayerAPI::RPCMode();
}

ScriptLanguage *LuaScriptInstance::get_language() {
    return nullptr;
}

void LuaScriptInstance::tmpLuaDebug(const char *fname )
{
    sol::state* lua = LuaScriptLanguage::get_singleton()->get_state();
    FILE *fp;
    char *str;
    char txt[1000];
    fname = "luadebug.lua";
    int filesize;
    if ((fp=fopen(fname,"r"))==NULL){
        printf("open file %s error\n",fname);
        return ;
    }

    fseek(fp,0,SEEK_END);

    filesize = ftell(fp);
    str=(char *)malloc(filesize);
    str[0]=0;

    rewind(fp);
    while((fgets(txt,1000,fp))!=NULL){
        strcat(str,txt);
    }
    fclose(fp);

    lua->safe_script(str);
}

int LuaScriptInstance::setup() {
    sol::state* lua = LuaScriptLanguage::get_singleton()->get_state();
    lua->set("print",print_luadebug);

	// luaL_newmetatable(L, "LuaObject");
	// {
	// 	static luaL_Reg meta_methods[] = {
	// 		{ "__gc", meta__gc },
	// 		{ "__index", meta__index },
	// 		{ "__newindex", meta__newindex },
	// 		{ "__tostring", meta__tostring },
	// 		{ NULL, NULL },
	// 	};
	// 	luaL_setfuncs(L, meta_methods, 0);

	// 	lua_newtable(L);
	// 	static luaL_Reg methods[] = {
	// 		{ "extends", l_extends },
	// 		{ NULL, NULL },
	// 	};
	// 	luaL_setfuncs(L, methods, 0);
	// 	lua_setfield(L, -2, ".methods");
	// }
	// lua_pop(L, 1);

	// luaL_newmetatable(L, "LuaVariant");
	// {
	// 	typedef struct {
	// 		const char *meta;
	// 		Variant::Operator op;
	// 	} eval;

	// 	static eval evaluates[] = {
	// 		{ "__eq", Variant::OP_EQUAL },
	// 		{ "__add", Variant::OP_ADD },
	// 		{ "__sub", Variant::OP_SUBTRACT },
	// 		{ "__mul", Variant::OP_MULTIPLY },
	// 		{ "__div", Variant::OP_DIVIDE },
	// 		{ "__mod", Variant::OP_MODULE },
	// 		{ "__lt", Variant::OP_LESS },
	// 		{ "__le", Variant::OP_LESS_EQUAL },
	// 	};

	// 	for (int idx = 0; idx < sizeof(evaluates) / sizeof(evaluates[0]); idx++) {

	// 		eval &ev = evaluates[idx];
	// 		lua_pushstring(L, ev.meta);
	// 		lua_pushinteger(L, ev.op);
	// 		lua_pushcclosure(L, meta_bultins__evaluate, 1);
	// 		lua_rawset(L, -3);
	// 	}

	// 	static luaL_Reg meta_methods[] = {
	// 		{ "__gc", meta_bultins__gc },
	// 		{ "__index", meta_bultins__index },
	// 		{ "__newindex", meta_bultins__newindex },
	// 		{ "__tostring", meta_bultins__tostring },
	// 		{ "__pairs", meta_bultins__pairs },
	// 		{ NULL, NULL },
	// 	};
	// 	luaL_setfuncs(L, meta_methods, 0);

	// 	lua_newtable(L);
	// 	static luaL_Reg methods[] = {
	// 		{ NULL, NULL },
	// 	};
	// 	luaL_setfuncs(L, methods, 0);
	// 	lua_setfield(L, -2, ".methods");
	// }
	// lua_pop(L, 1);

	// /* create object ptr -> udata mapping table */
	// lua_pushstring(L, "gdlua_ubox");
	// lua_newtable(L);
	// /* make weak value metatable for ubox table to allow userdata to be
	//    garbage-collected */
	// lua_newtable(L);
	// lua_pushliteral(L, "__mode");
	// lua_pushliteral(L, "v");
	// lua_rawset(L, -3); /* stack: string ubox mt */
	// lua_setmetatable(L, -2); /* stack: string ubox */
	// lua_rawset(L, LUA_REGISTRYINDEX);
    tmpLuaDebug("luadebug.lua");
	return 0;
}
//=================================
