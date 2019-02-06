#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance()
{

}

LuaScriptInstance::~LuaScriptInstance()
{

}

Ref<Script> LuaScriptInstance::get_script() const
{
    return script;
}

ScriptLanguage *LuaScriptInstance::get_language()
{
    return LuaScriptLanguage::get_singleton();
}
