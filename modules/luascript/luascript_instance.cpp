#include "luascript_instance.h"
#include "luascript_language.h"

LuaScriptInstance::LuaScriptInstance() {
	lua_ref = 0;
	base_ref = false;
}

LuaScriptInstance::~LuaScriptInstance() {
	LuaScriptLanguage::get_singleton()->binding->l_unref_instance(this);
}

Error LuaScriptInstance::initialize(const Variant **p_args, int p_argcount, bool p_ref) {
	Variant::CallError err;
	LuaScript *p_spt = script.ptr();
	while (p_spt) {
		if (p_spt->has_method("_init")) {
			LuaScriptLanguage::get_singleton()->binding->instance_call(this, "_init", p_args, p_argcount, err);
			break;
		}
		p_spt = p_spt->_base;
	}
	return OK;
}

bool LuaScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	LuaScript *p_spt = script.ptr();
	while (p_spt) {
		//don't cover the script method is ok to set in script
		if (!p_spt->methods_name.has(p_name)) {
			return LuaScriptLanguage::get_singleton()->binding->l_instance_set(this, p_name, p_value);
		}
		p_spt = p_spt->_base;
	}
	bool success = false;
	ClassDB::set_property(owner, p_name, p_value, &success);
	return success;
}

bool LuaScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	const LuaScript *p_spt = script.ptr();
	while (p_spt) {
		if (p_spt->properties_default_value.has(p_name)) {
			return LuaScriptLanguage::get_singleton()->binding->l_instance_get(this, p_name, r_ret);
		}
		p_spt = p_spt->_base;
	}
	bool success = ClassDB::get_property(owner, p_name, r_ret);
	return success;
}

/*TODO*/ void LuaScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
}

/*TODO*/ Variant::Type LuaScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
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
	return ret;
}

Variant LuaScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	LuaScript *p_spt = script.ptr();
	while (p_spt) {
		if (p_spt->has_method(p_method)) {
			return LuaScriptLanguage::get_singleton()->binding->instance_call(this, p_method, p_args, p_argcount, r_error);
		}
		p_spt = p_spt->_base;
	}

	//find object in cls
	const ClassDB::ClassInfo *top = script->cls;
	while (top) {
		if (top->method_map.has(p_method)) {
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
