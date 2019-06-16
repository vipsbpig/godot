#ifndef LUABINDINGHELPER_H
#define LUABINDINGHELPER_H


#include "lua.hpp"
#include "core/object.h"

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM==501
/*
** Adapted from Lua 5.2.0
*/
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}
#endif

class LuaBindingHelper
{
    lua_State * L;

private:
    static int l_print(lua_State *L);

    // lua methods
    static int l_extends(lua_State *L);

    //native binding
    static int create_user_data(lua_State *L);

    // LuaObject lua meta methods
    static int meta__gc(lua_State *L);
    static int meta__tostring(lua_State *L);
    static int meta__index(lua_State *L);
    static int meta__newindex(lua_State *L);

    void openLibs(lua_State *L);
    void globalbind();
    void register_class(lua_State *L ,const ClassDB::ClassInfo* cls);
public:
    LuaBindingHelper();

    void initialize();
    void uninitialize();

    Error script(const String &p_source);

};

#endif // LUABINDINGHELPER_H
