#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance() {
}

LuaScriptInstance::~LuaScriptInstance() {
	print_format("LuaScriptInstance::descontruct");
}

Error LuaScriptInstance::init(bool p_ref) {
	//TODO::调用lua的类下面的构造函数
	//把script 的 luaref存在instance下面
	//然后方便后续的调用
	//script里面获取方法，instance的对象来调用
	//这里要父类调用设置叫base吧
	//需要创建LuaInstance的metatable ，用table还是userdata呢？
	//不能创建新的变量，但是可以赋值
	//赋值可以传递到引擎里面
	//如果复制到自己就引擎就不可以得知

	Object *obj = owner;
	print_format("LuaScriptInstance::init obj type:%s script:%d", String(Variant(obj)).utf8().get_data(), script.ptr());
	//call("_init", NULL, 0, Variant::CallError());
	return OK;
}
bool LuaScriptInstance::has_method(const StringName &p_method) const {
	bool ret = script->has_method(p_method);
	if (ret || script->cls != NULL)
		ret = script->cls->method_map.has(p_method);
	if (ret || script->_base != NULL)
		ret = script->_base->has_method(p_method);
	print_format("LuaScriptInstance::has_method %s ret:%s", String(p_method).utf8().get_data(), ret ? "true" : "false");

	return ret;
}

Variant LuaScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	//TODO:: need to complete
	print_format("LuaScriptInstance::call %s %d", String(p_method).utf8().get_data(), p_argcount);
	if (script->has_method(p_method)) {
		Variant &&var = LuaScriptLanguage::get_singleton()->binding->instance_call(this, p_method, p_args, p_argcount, r_error);
		return var;
	}

	//find object in cls
	const ClassDB::ClassInfo *top = script->cls;
	while (top->inherits_ptr) {
		if (top->method_map.has(p_method)) {
			MethodBind *mb = top->method_map[p_method];
			return mb->call(owner, p_args, p_argcount, r_error);
		}
		top = top->inherits_ptr;
	}

	return Variant();
}

Ref<Script> LuaScriptInstance::get_script() const {
	return script;
}

ScriptLanguage *LuaScriptInstance::get_language() {
	return LuaScriptLanguage::get_singleton();
}
