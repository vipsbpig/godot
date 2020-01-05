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

int LuaBindingHelper::l_extends(lua_State *L)
{
    print_format("call %s", "l_extends");
    return 0;
}

int LuaBindingHelper::create_user_data(lua_State *L)
{
    const ClassDB::ClassInfo *cls = (ClassDB::ClassInfo *)lua_touserdata(L, lua_upvalueindex(1));

    const StringName *key = cls->method_map.next(NULL);

    lua_pushstring(L, "gdlua_ubox");
    lua_rawget(L, LUA_REGISTRYINDEX);
    {
        Object** ud  = NULL;
        ud = (Object** )lua_newuserdata(L, sizeof(Object*));
        Object* object = cls->creation_func();
        print_format("Name %s call new object_ptr:%d ", object->get_class().ascii().get_data() , object);

        *ud = object;
        luaL_getmetatable(L,"LuaObject");
        lua_setmetatable(L,-2);
        lua_pushvalue(L,-1);
        lua_pushvalue(L,-1);
        lua_rawset(L,-4);
        lua_remove(L,1);
    }
//    while (key) {
//        print_format("-- methods:%s", String(*key).ascii().get_data() );
//        key = cls->method_map.next(key);
//    }
    return 1;
}

int LuaBindingHelper::meta__gc(lua_State *L)
{
    Object** ud = (Object**)lua_touserdata(L,1);
    Object* obj = *ud;
    String toString = "[" +obj->get_class()+ ":" + itos( obj->get_instance_id()) +"]";
    print_format("%s memdeted by lua gc." , toString.ascii().get_data());
    memdelete(obj);
    return 0;

}

int LuaBindingHelper::meta__tostring(lua_State *L)
{
    print_format("meta__tostring");
    Object** ud = (Object**)lua_touserdata(L,1);
    Object* obj = *ud;
    String toString = "[" +obj->get_class()+ ":" + itos( obj->get_instance_id()) +"]";
    lua_pushstring(L, toString.ascii().get_data() );
    return 1;

}

int LuaBindingHelper::meta__index(lua_State *L)
{
    Object** ud = (Object**)lua_touserdata(L,1);
    Object* obj = *ud;
    String index_name = lua_tostring(L,2);
    print_format("meta__index: %s call %s",  obj->get_class().ascii().get_data() ,index_name.ascii().get_data());

    return 0;

}

int LuaBindingHelper::meta__newindex(lua_State *L)
{
    print_format("call %s", "meta__newindex");
    return 0;

}

void LuaBindingHelper::openLibs(lua_State* L)
{
    luaL_Reg lualibs[] = {
        {"", luaopen_base},
        //{LUA_LOADLIBNAME, luaopen_package},
        //{LUA_TABLIBNAME, luaopen_table},
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
    lua_pushcfunction(L,l_print);
    lua_setglobal(L,"println");

}

void LuaBindingHelper::register_class(lua_State *L, const ClassDB::ClassInfo *cls)
{
    if (!( String(cls->name) == "Object" || String(cls->name) == "Node"))

        return;

    //print_format("stack size = %d", lua_gettop(L));
    print_format("%s:%s",String(cls->name).ascii().get_data(),String(cls->inherits).ascii().get_data());

    CharString s = String(cls->name).ascii();
    const char *typeName = s.get_data();
    print_format("0-stack size = %d", lua_gettop(L));

    lua_newtable(L);

    lua_pushvalue(L,-1);
    lua_setglobal(L,typeName);
    print_format("1-stack size = %d ,push %s in to global", lua_gettop(L),typeName);

    //lua_pushvalue(L,-1);
    lua_pushlightuserdata(L, (void*) cls );
    lua_pushcclosure( L, create_user_data, 1 );
    lua_setfield( L, -2, "new" );

    lua_pop(L,1);
}

void LuaBindingHelper::initialize()
{
    L = luaL_newstate();
    luaopen_base(L);
    luaopen_table(L);
    luaopen_debug(L);
    lua_settop(L,0);

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

    print_format("stack size = %d", lua_gettop(L));

    /* create object ptr -> udata mapping table */
    lua_pushstring(L, "gdlua_ubox");
    lua_newtable(L);
    /* make weak value metatable for ubox table to allow userdata to be
       garbage-collected */
    lua_newtable(L);
    lua_pushliteral(L, "__mode");
    lua_pushliteral(L, "kv");
    lua_rawset(L, -3);			   /* stack: string ubox mt */
    lua_setmetatable(L, -2);  /* stack: string ubox */
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

    print_format("stack size = %d", lua_gettop(L));

    //===================
    //    LuaTestParent* parent = new LuaTestParent();
    //    auto* child = new LuaTestChild();
    //    child->funcC(parent);

    //    LuaTestParent p;
    //    child->funcD(p);

}

void LuaBindingHelper::uninitialize()
{
    lua_close(L);
    L = NULL;
}

Error LuaBindingHelper::script(const String &p_source)
{
    ERR_FAIL_NULL_V(L,ERR_DOES_NOT_EXIST);
    luaL_loadstring(L, p_source.utf8());
    if ( lua_pcall(L, 0, LUA_MULTRET, 0)){
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
        lua_pushvalue(L, 1);  /* pass error message */
        lua_pushinteger(L, 2);  /* skip this function and traceback */
        lua_call(L, 2, 1);  /* call debug.traceback */
        print_line(lua_tostring(L, -1));
        return ERR_SCRIPT_FAILED;
    }

    return OK;
}
