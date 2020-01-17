#include "luascript.h"
#include "core/os/file_access.h"
#include "luascript_instance.h"
#include "luascript_language.h"

LuaScript::LuaScript() {
	_base = NULL;
	cls = NULL;
	valid = false;
	tool = false;
	lua_ref = 0;
}

LuaScript::~LuaScript() {
	LuaScriptLanguage::get_singleton()->binding->l_unref_luascript(this);
}

ScriptInstance *LuaScript::_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_isref, Variant::CallError &r_error) {

	/* STEP 1, CREATE */
	LuaScriptInstance *instance = memnew(LuaScriptInstance);
	instance->base_ref = p_isref;
	//	instance->gc_delete=true;
	//	instance->members.resize(member_indices.size());
	instance->script = Ref<LuaScript>(this);
	instance->owner = p_owner;
	instance->owner->set_script_instance(instance);
	LuaScriptLanguage::get_singleton()->binding->helper_push_instance(instance);

	/* STEP 2, INITIALIZE AND CONSRTUCT */
	instances.insert(instance->owner);

	if (instance->initialize(p_args, p_argcount, p_isref) != OK) {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
		instance->script = Ref<LuaScript>();
		instance->owner->set_script_instance(NULL);

		memdelete(instance);
		instances.erase(p_owner);

		ERR_FAIL_COND_V(r_error.error != Variant::CallError::CALL_OK, NULL); //error constructing
	}

	//@TODO make thread safe
	return instance;
}

bool LuaScript::can_instance() const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::can_instance");
#endif
	return valid;
}
StringName LuaScript::get_instance_base_type() const {
	if (native.is_valid())
		return native->get_name();
	if (base.is_valid())
		return _base->get_instance_base_type();
	if (cls != NULL) {
		return cls->name;
	}
	return StringName();
}
ScriptInstance *LuaScript::instance_create(Object *p_this) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::instance_create");
#endif

	LuaScript *top = this;
	while (top->_base)
		top = top->_base;

	if (top->native.is_valid()) {
		if (!ClassDB::is_parent_class(p_this->get_class_name(), top->native->get_name())) {

			// if (ScriptDebugger::get_singleton()) {
			// 	LuaScriptLanguage::get_singleton()->debug_break_parse(get_path(), 0, "Script inherits from native type '" + String(top->native->get_name()) + "', so it can't be instanced in object of type: '" + p_this->get_class() + "'");
			// }
			ERR_EXPLAIN("Script inherits from native type '" + String(top->native->get_name()) + "', so it can't be instanced in object of type: '" + p_this->get_class() + "'");
			ERR_FAIL_V(NULL);
		}
	}

	Variant::CallError unchecked_error;
	return _create_instance(NULL, 0, p_this, Object::cast_to<Reference>(p_this), unchecked_error);
}

ScriptLanguage *LuaScript::get_language() const {
	return LuaScriptLanguage::get_singleton();
}

String LuaScript::get_source_code() const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::get_source_code");
#endif

	return source;
}

void LuaScript::set_source_code(const String &p_code) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::set_source_code");
#endif
	if (source == p_code)
		return;
	source = p_code;
	// #ifdef TOOLS_ENABLED
	// 	source_changed_cache = true;
	// #endif;
}

Error LuaScript::reload(bool p_keep_state) {
	//完成绑定等内容和调整
	//传入调用等内容
	//extends 的设计 是谁的
	//只要是绑定到node的可以通过
	//临时创建extendts来绑定到脚本类上面
	//只会在reload的时候进行更替，在引擎运行之前都会正确调用

#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::reload");
#endif

	bool has_instances = instances.size();
	ERR_FAIL_COND_V(!p_keep_state && has_instances, ERR_ALREADY_IN_USE);
	String basedir = path;

	if (basedir == "")
		basedir = get_path();

	if (basedir != "")
		basedir = basedir.get_base_dir();

	LuaScriptLanguage::get_singleton()->binding->bind_script_function("extends", this, LuaBindingHelper::l_extends);
	Error err = LuaScriptLanguage::get_singleton()->binding->script(source);
	LuaScriptLanguage::get_singleton()->binding->unbind_script_function("extends");
	valid = (err == OK);
	return err;
}

void LuaScript::add_lua_method(const StringName &method_name) {
	if (methods_name.has(method_name)) return;
	methods_name.insert(method_name);
}
bool LuaScript::has_method(const StringName &p_method) const {
	return methods_name.has(p_method);
}
// bool LuaScript::has_method_with_cls(const String &p_method) const {
// 	bool ret = has_method(p_method);
// 	if (has_method(p_method) || cls == NULL)
// 		return ret;
// 	else {
// 		auto *top = this->cls;
// 		while (top) {
// 			if (top->method_map.has(p_method))
// 				return true;
// 			top = top->inherits_ptr;
// 		}
// 		return false;
// 	}
// }
void LuaScript::add_property_default_value(const StringName &p_property, const Variant &p_value) {
	properties_default_value[p_property] = p_value;
}
bool LuaScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	if (!properties_default_value.has(p_property)) return false;
	r_value = properties_default_value[p_property];
	return true;
}
void LuaScript::add_lua_property_type(const StringName &name, int type) {
	lua_properties_type[name] = type;
}
Error LuaScript::load_source_code(const String &p_path) {

#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScript::load_source_code");
#endif

	//把脚本传进来加载内容
	PoolVector<uint8_t> sourcef;
	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ, &err);
	if (err) {

		ERR_FAIL_COND_V(err, err);
	}

	int len = f->get_len();
	sourcef.resize(len + 1);
	PoolVector<uint8_t>::Write w = sourcef.write();
	int r = f->get_buffer(w.ptr(), len);
	f->close();
	memdelete(f);
	ERR_FAIL_COND_V(r != len, ERR_CANT_OPEN);
	w[len] = 0;

	String s;
	if (s.parse_utf8((const char *)w.ptr())) {

		ERR_EXPLAIN("Script '" + p_path + "' contains invalid unicode (utf-8), so it was not loaded. Please ensure that scripts are saved in valid utf-8 unicode.");
		ERR_FAIL_V(ERR_INVALID_DATA);
	}

	source = s;
	// #ifdef TOOLS_ENABLED
	// 	source_changed_cache = true;
	// #endif
	path = p_path;
	return OK;
}

//========luascript loader=========

Ref<Resource> LuaScriptResourceFormatLoader::load(const String &p_path, const String &p_original_path, Error *r_error) {

#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatLoader::load( p_path = " + p_path + ", p_original_path = " + p_original_path + " )");
#endif
	LuaScript *script = memnew(LuaScript);
	if (!script) {
		if (r_error)
			*r_error = ERR_FILE_CANT_OPEN;
	}

	if (p_path.ends_with(".luac")) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
		print_debug("LuaScriptResourceFormatLoader::load luajit");
#endif
		ERR_FAIL_V(RES());
		//TODO:: luajit
		//Error err = script->load_byte_code(p_path);
		//ERR_FAIL_COND_V(err != OK, RES());
		//script->set_path(p_original_path);
		//script->reload();
	} else {
		Error err = script->load_source_code(p_path);
		ERR_FAIL_COND_V(err != OK, RES());
		script->set_path(p_original_path);

		script->reload();
	}
	if (r_error)
		*r_error = OK;
	return Ref<LuaScript>(script);
}

void LuaScriptResourceFormatLoader::get_recognized_extensions(List<String> *p_extensions) const {
	//print_debug("LuaScriptResourceFormatLoader::get_recognized_extensions");

	p_extensions->push_back("lua");
	//TODO::luajit
	//p_extensions->push_back("luac");
}

bool LuaScriptResourceFormatLoader::handles_type(const String &p_type) const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatLoader::handles_type( p_type = " + p_type + " )");
#endif

	return (p_type == "Script" || p_type == "LuaScript");
}

String LuaScriptResourceFormatLoader::get_resource_type(const String &p_path) const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatLoader::get_resource_type( p_path = " + p_path + " )");
#endif
	String el = p_path.get_extension().to_lower();
	if (el == "lua") //luajit || el == LUAJIT_EXTENSION )
		return "LuaScript";
	return "";
}

Error LuaScriptResourceFormatSaver::save(const String &p_path, const Ref<Resource> &p_resource, uint32_t p_flags) {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatSaver::save( p_path = " + p_path + " )");
#endif
	Ref<LuaScript> script = p_resource;

	ERR_FAIL_COND_V(script.is_null(), ERR_INVALID_PARAMETER);

	String source = script->get_source_code();

	Error error;
	FileAccess *file = FileAccess::open(p_path, FileAccess::WRITE, &error);

	if (error != OK) return error;

	file->store_string(source);
	if (file->get_error() != OK && file->get_error() != ERR_FILE_EOF) {
		memdelete(file);
		return ERR_CANT_CREATE;
	}
	file->close();
	memdelete(file);

	if (ScriptServer::is_reload_scripts_on_save_enabled()) {
		LuaScriptLanguage::get_singleton()->reload_tool_script(p_resource, false);
	}

	return OK;
}

void LuaScriptResourceFormatSaver::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatSaver::get_recognized_extensions");
#endif
	if (Object::cast_to<LuaScript>(*p_resource)) {
		p_extensions->push_back("lua");
		//TODO:: luajit
		//p_extensions->push_back(LUAJIT_EXTENSION);
	}
}

bool LuaScriptResourceFormatSaver::recognize(const Ref<Resource> &p_resource) const {
#ifdef LUA_SCRIPT_DEBUG_ENABLED
	print_debug("LuaScriptResourceFormatSaver::recognize");
#endif
	return Object::cast_to<LuaScript>(*p_resource) != nullptr;
}
