#ifndef LUABINDINGHELPER_H
#define LUABINDINGHELPER_H


#include "lua.hpp"
#include "core/object.h"

class LuaBindingHelper
{
    lua_State * L;

private:
    static int l_print(lua_State *L);

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
