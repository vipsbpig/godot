#include "luabinding_helper.h"
#include "../debug.h"


LuaBindingHelper::LuaBindingHelper() :L(NULL)
{

}

int LuaBindingHelper::l_print(lua_State *L)
{
    const char* str = luaL_checkstring(L,1);
    print_line(str);
    return 0;
}

void LuaBindingHelper::openLibs(lua_State* L)
{
    luaL_Reg lualibs[] = {
      {"", luaopen_base},
      //{LUA_LOADLIBNAME, luaopen_package},
      {LUA_TABLIBNAME, luaopen_table},
      //{LUA_IOLIBNAME, luaopen_io},
      //{LUA_OSLIBNAME, luaopen_os},
      //{LUA_STRLIBNAME, luaopen_string},
      //{LUA_MATHLIBNAME, luaopen_math},
      {LUA_DBLIBNAME, luaopen_debug},
      //{"lua-utf8", luaopen_utf8},
      {NULL, NULL}
    };

    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++) {
        luaL_register(L,lib->name,lib);
    }

}



void LuaBindingHelper::globalbind()
{
    print_format("-1stack size = %d", lua_gettop(L));

    lua_pushcfunction(L,l_print);
    print_format("-2stack size = %d", lua_gettop(L));

    lua_setglobal(L,"println");
    print_format("-3stack size = %d", lua_gettop(L));

}

void LuaBindingHelper::register_class(lua_State *L, const ClassDB::ClassInfo *cls)
{
    print_format("0stack size = %d", lua_gettop(L));
    //为了下面注册函数用
    lua_newtable(L);
    int methods = lua_gettop(L);
    print_format("1stack size = %d", lua_gettop(L));

    //注册metatable为雷鸣
    luaL_newmetatable(L, String(cls->name).ascii().get_data());
    int metatable = lua_gettop(L);
    print_format("2stack size = %d", lua_gettop(L));

    lua_pushstring(L, String(cls->name).ascii().get_data());
    print_format("3stack size = %d", lua_gettop(L));
    lua_pushvalue(L,methods);
    print_format("4stack size = %d", lua_gettop(L));
    lua_settable(L,LUA_GLOBALSINDEX);
    print_format("5stack size = %d", lua_gettop(L));

    lua_pushliteral(L,"__metatable");
    lua_pushvalue(L,methods);
    lua_settable(L,metatable);




}

void LuaBindingHelper::initialize()
{
    L = luaL_newstate();
    openLibs(L);
    //global function
    globalbind();
    // register class
    {
        const StringName *key = ClassDB::classes.next(NULL);
        while (key) {
            const ClassDB::ClassInfo *cls = ClassDB::classes.getptr(*key);
            register_class(L, cls);
            key = ClassDB::classes.next(key);
            return;
        }
    }

}

void LuaBindingHelper::uninitialize()
{
    lua_close(L);
    L = NULL;
}

Error LuaBindingHelper::script(const String &p_source)
{
    ERR_FAIL_NULL_V(L,ERR_DOES_NOT_EXIST);

    if (luaL_dostring(L,p_source.utf8()))
        return OK;
    else
        return ERR_SCRIPT_FAILED;
}
