#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance() {
}

LuaScriptInstance::~LuaScriptInstance() {
}

Error LuaScriptInstance::init(bool p_ref) {
	//TODO::调用lua的类下面的构造函数
	//把对象存在instance下面
	//然后方便后续的调用
	//

	return OK;
}



Ref<Script> LuaScriptInstance::get_script() const {
	return script;
}

ScriptLanguage *LuaScriptInstance::get_language() {
	return LuaScriptLanguage::get_singleton();
}
