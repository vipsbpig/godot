#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance() {
	lua_ref = 0;
	base_ref = false;
}

LuaScriptInstance::~LuaScriptInstance() {
	print_format("LuaScriptInstance::descontruct");
	LuaScriptLanguage::get_singleton()->binding->l_unref_instance(this);
}

// Error LuaScriptInstance::initialize(const Variant **p_args, int p_argcount, bool p_ref) {
// 	Variant::CallError err;
// 	call("_init", p_args, p_argcount, err);
// 	return err.error == Variant::CallError::CALL_OK ? OK : FAILED;
// }

bool LuaScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	print_debug("LuaScriptInstance::set");
	return false;
}

bool LuaScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	print_debug("LuaScriptInstance::get");
	return false;
}

/*TODO*/ void LuaScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
	print_debug("LuaScriptInstance::get_property_list");
}

/*TODO*/ Variant::Type LuaScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	print_debug("LuaScriptInstance::get_property_type");
	return Variant::NIL;
}

bool LuaScriptInstance::has_method(const StringName &p_method) const {
	const LuaScript *p_spt = script.ptr();
	bool ret = false;
	while (p_spt) {
		if (p_spt->has_method(p_method)) {
			ret = true;
			break;
		}
		p_spt = p_spt->_base;
	}
	print_format("LuaScriptInstance::has_method %s ret:%s", String(p_method).ascii().get_data(), ret ? "true" : "false");
	return ret;
}

Variant LuaScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	LuaScript *p_spt = script.ptr();
	while (p_spt) {
		if (p_spt->has_method(p_method)) {
			print_format("LuaScript::call %s %d", String(p_method).ascii().get_data(), p_argcount);
			return LuaScriptLanguage::get_singleton()->binding->instance_call(this, p_method, p_args, p_argcount, r_error);
		}
		p_spt = p_spt->_base;
	}

	//find object in cls
	const ClassDB::ClassInfo *top = script->cls;
	while (top) {
		if (top->method_map.has(p_method)) {
			print_format("LuaScript::call cls %d:%s argc:%d", String(top->name).ascii().get_data(), String(p_method).ascii().get_data(), p_argcount);
			MethodBind *mb = top->method_map[p_method];
			return mb->call(owner, p_args, p_argcount, r_error);
		}
		top = top->inherits_ptr;
	}
	r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	return Variant();
}

Ref<Script> LuaScriptInstance::get_script() const {
	return script;
}

ScriptLanguage *LuaScriptInstance::get_language() {
	return LuaScriptLanguage::get_singleton();
}
