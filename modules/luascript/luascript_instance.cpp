#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance() {
}

LuaScriptInstance::~LuaScriptInstance() {
}

Error LuaScriptInstance::init(bool p_ref) {
	//TODO::调用lua的类下面的构造函数
	//把script 的 luaref存在instance下面
	//然后方便后续的调用
	//script里面获取方法，instance的对象来调用

	Object *obj = owner;
	print_format("LuaScriptInstance::init obj type:%s script:%d", String(Variant(obj)).utf8().get_data(), script.ptr());

	//print_format("class [%s]instance init call.",String(get_script()->get_instance_base_type()).utf8().get_data());
	return OK;
}

Ref<Script> LuaScriptInstance::get_script() const {
	return script;
}

ScriptLanguage *LuaScriptInstance::get_language() {
	return LuaScriptLanguage::get_singleton();
}
