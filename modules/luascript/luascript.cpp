/*
 * This file is part of LuaScript
 * https://github.com/perbone/luascrip/
 *
 * Copyright 2017-2018 Paulo Perbone
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not  use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

#include "core/engine.h"
#include "core/global_constants.h"
#include "core/io/file_access_encrypted.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "core/project_settings.h"

#include "constants.h"
#include "debug.h"
#include "luascript.h"


bool LuaNativeClass::_get(const StringName &p_name, Variant &r_ret) const {
	bool ok;
	int v = ClassDB::get_integer_constant(name, p_name, &ok);

	if (ok) {
		r_ret = v;
		return true;
	} else {
		return false;
	}
	return false;
}

void LuaNativeClass::_bind_methods() {
	ClassDB::bind_method(D_METHOD("new"),&LuaNativeClass::_new);
}

Variant LuaNativeClass::_new() {
	Object *o = instance();
	if (!o) {
		ERR_EXPLAIN("Class type: '" + String(name) + "' is not instantiable.");
		ERR_FAIL_COND_V(!o, Variant());
	}

	Reference *ref = Object::cast_to<Reference>(o);
	if (ref) {
		print_debug("LuaNativeClass::_new Success");
		return REF(ref);
	} else {
		print_debug("LuaNativeClass::_new Failed");
		return o;
	}
}

Object *LuaNativeClass::instance() {
	return ClassDB::instance(name);
}

LuaNativeClass::LuaNativeClass(const StringName &p_name) {
	name = p_name;
}


//=================LUAScript==========
LuaScriptInstance *LuaScript::_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_isref) {
	return nullptr;
}

void LuaScript::_placeholder_erased(PlaceHolderScriptInstance *p_placeholder) {
}

void LuaScript::_update_exports_values(Map<StringName, Variant> &values, List<PropertyInfo> &propnames) {
}

bool LuaScript::_get(const StringName &p_name, Variant &r_ret) const {
	return false;
}

bool LuaScript::_set(const StringName &p_name, const Variant &p_value) {
	return false;
}

void LuaScript::_get_property_list(List<PropertyInfo> *p_properties) const {
}

Variant LuaScript::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	return Variant();
}

void LuaScript::_bind_methods() {
}

Ref<LuaScript> LuaScript::get_base() const {
	return Ref<LuaScript>();
}

bool LuaScript::can_instance() const {
	return false;
}

Ref<Script> LuaScript::get_base_script() const {
	return Ref<Script>();
}

StringName LuaScript::get_instance_base_type() const {
	return StringName();
}

ScriptInstance *LuaScript::instance_create(Object *p_this) {
	return nullptr;
}

bool LuaScript::instance_has(const Object *p_this) const {
	return false;
}

bool LuaScript::has_source_code() const {
	return false;
}

String LuaScript::get_source_code() const {
	return String();
}

void LuaScript::set_source_code(const String &p_code) {
}

Error LuaScript::reload(bool p_keep_state) {
	return Error();
}

bool LuaScript::has_method(const StringName &p_method) const {
	return false;
}

MethodInfo LuaScript::get_method_info(const StringName &p_method) const {
	return MethodInfo();
}

ScriptLanguage *LuaScript::get_language() const {
	return nullptr;
}

bool LuaScript::has_script_signal(const StringName &p_signal) const {
	return false;
}

void LuaScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
}

bool LuaScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	return false;
}

void LuaScript::update_exports() {
}

void LuaScript::get_script_method_list(List<MethodInfo> *p_list) const {
}

void LuaScript::get_script_property_list(List<PropertyInfo> *p_list) const {
}

int LuaScript::get_member_line(const StringName &p_member) const {
	return 0;
}

void LuaScript::get_constants(Map<StringName, Variant> *p_constants) {
}

void LuaScript::get_members(Set<StringName> *p_constants) {
}

Error LuaScript::load_byte_code(const String &p_path) {
	return Error();
}

Error LuaScript::load_source_code(const String &p_path) {
	return Error();
}

Variant LuaScript::_new(const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	return Variant();
}


LuaScript::LuaScript(){
}

LuaScript::~LuaScript() {
}
//=================LUAScript==========

//===========LUAScritpLanguage=======
LuaScriptLanguage *LuaScriptLanguage::singleton = nullptr;

void LuaScriptLanguage::_add_global(const StringName &p_name, const Variant &p_value) {
	if (globals.has(p_name)) {
		//overwrite existing
		global_array.write[globals[p_name]] = p_value;
		return;
	}
	globals[p_name] = global_array.size();
	global_array.push_back(p_value);
	_global_array = global_array.ptrw();
}

LuaScriptLanguage::LuaScriptLanguage() {
}

LuaScriptLanguage::~LuaScriptLanguage() {
}

String LuaScriptLanguage::get_name() const {
	return String();
}

void LuaScriptLanguage::init() {

	// setup lua instance object's metamethods
	LuaScriptInstance::setup();
	//populate global constants
	int gcc = GlobalConstants::get_global_constant_count();
	for (int i = 0; i < gcc; i++) {

		_add_global(StaticCString::create(GlobalConstants::get_global_constant_name(i)), GlobalConstants::get_global_constant_value(i));
	}

	_add_global(StaticCString::create("PI"), Math_PI);
	_add_global(StaticCString::create("TAU"), Math_TAU);
	_add_global(StaticCString::create("INF"), Math_INF);
	_add_global(StaticCString::create("NAN"), Math_NAN);

	//populate native classes

	List<StringName> class_list;
	ClassDB::get_class_list(&class_list);
	for (List<StringName>::Element *E = class_list.front(); E; E = E->next()) {

		StringName n = E->get();
		String s = String(n);
		if (s.begins_with("_"))
			n = s.substr(1, s.length());

		if (globals.has(n))
			continue;
		Ref<LuaNativeClass> nc = memnew(LuaNativeClass(E->get()));
		_add_global(n, nc);
	}

	//populate singletons

	List<Engine::Singleton> singletons;
	Engine::get_singleton()->get_singletons(&singletons);
	for (List<Engine::Singleton>::Element *E = singletons.front(); E; E = E->next()) {

		_add_global(E->get().name, E->get().ptr);
	}
}

String LuaScriptLanguage::get_type() const {
	return "";
}

String LuaScriptLanguage::get_extension() const {
	return String();
}

Error LuaScriptLanguage::execute_file(const String &p_path) {
	return Error();
}

void LuaScriptLanguage::finish() {
}

void LuaScriptLanguage::get_reserved_words(List<String> *p_words) const {
}

void LuaScriptLanguage::get_comment_delimiters(List<String> *p_delimiters) const {
}

void LuaScriptLanguage::get_string_delimiters(List<String> *p_delimiters) const {
}

Ref<Script> LuaScriptLanguage::get_template(const String &p_class_name, const String &p_base_class_name) const {
	return Ref<Script>();
}

void LuaScriptLanguage::make_template(const String &p_class_name, const String &p_base_class_name, Ref<Script> &p_script) {
}

bool LuaScriptLanguage::is_using_templates() {
	return false;
}

bool LuaScriptLanguage::validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path, List<String> *r_functions, List<Warning> *r_warnings, Set<int> *r_safe_lines) const {
	return false;
}

String LuaScriptLanguage::validate_path(const String &p_path) const {
	return String();
}

Script *LuaScriptLanguage::create_script() const {
	return nullptr;
}

bool LuaScriptLanguage::has_named_classes() const {
	return false;
}

bool LuaScriptLanguage::supports_builtin_mode() const {
	return false;
}

bool LuaScriptLanguage::can_inherit_from_file() {
	return false;
}

int LuaScriptLanguage::find_function(const String &p_function, const String &p_code) const {
	return 0;
}

String LuaScriptLanguage::make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const {
	return String();
}

Error LuaScriptLanguage::open_in_external_editor(const Ref<Script> &p_script, int p_line, int p_col) {
	return Error();
}

bool LuaScriptLanguage::overrides_external_editor() {
	return false;
}

Error LuaScriptLanguage::complete_code(const String &p_code, const String &p_base_path, Object *p_owner, List<String> *r_options, bool &r_force, String &r_call_hint) {
	return Error();
}

Error LuaScriptLanguage::lookup_code(const String &p_code, const String &p_symbol, const String &p_base_path, Object *p_owner, LookupResult &r_result) {
	return Error();
}

void LuaScriptLanguage::auto_indent_code(String &p_code, int p_from_line, int p_to_line) const {
}

void LuaScriptLanguage::add_global_constant(const StringName &p_variable, const Variant &p_value) {
}

void LuaScriptLanguage::thread_enter() {
}

void LuaScriptLanguage::thread_exit() {
}

String LuaScriptLanguage::debug_get_error() const {
	return String();
}

int LuaScriptLanguage::debug_get_stack_level_count() const {
	return 0;
}

int LuaScriptLanguage::debug_get_stack_level_line(int p_level) const {
	return 0;
}

String LuaScriptLanguage::debug_get_stack_level_function(int p_level) const {
	return String();
}

String LuaScriptLanguage::debug_get_stack_level_source(int p_level) const {
	return String();
}

void LuaScriptLanguage::debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

void LuaScriptLanguage::debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

ScriptInstance *LuaScriptLanguage::debug_get_stack_level_instance(int p_level) {
	return nullptr;
}

void LuaScriptLanguage::debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

String LuaScriptLanguage::debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems, int p_max_depth) {
	return String();
}


void LuaScriptLanguage::reload_all_scripts() {
}

void LuaScriptLanguage::reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) {
}

void LuaScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
}

void LuaScriptLanguage::get_public_functions(List<MethodInfo> *p_functions) const {
}

void LuaScriptLanguage::get_public_constants(List<Pair<String, Variant> > *p_constants) const {
}

void LuaScriptLanguage::profiling_start() {
}

void LuaScriptLanguage::profiling_stop() {
}

int LuaScriptLanguage::profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) {
	return 0;
}

int LuaScriptLanguage::profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) {
	return 0;
}

void *LuaScriptLanguage::alloc_instance_binding_data(Object *p_object) {
	return nullptr;
}

void LuaScriptLanguage::free_instance_binding_data(void *p_data) {
}

void LuaScriptLanguage::frame() {
}

bool LuaScriptLanguage::debug_break_parse(const String &p_file, int p_line, const String &p_error) {
	return false;
}
//=================
//========luascript loader=========

Ref<Resource> LuaScriptResourceFormatLoader::load(const String &p_path, const String &p_original_path, Error *r_error) {
	print_debug("LuaScriptResourceFormatLoader::load( p_path = " + p_path + ", p_original_path = " + p_original_path + " )");

	LuaScript *script = memnew(LuaScript);
	if (!script) {
		if (r_error)
			*r_error = ERR_FILE_CANT_OPEN;
	}

	if (p_path.ends_with(".luac")) {
		print_debug("LuaScriptResourceFormatLoader::load luajit");
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
	print_debug("LuaScriptResourceFormatLoader::get_recognized_extensions");

	p_extensions->push_back(LUA_EXTENSION);
	//TODO::luajit 
	//p_extensions->push_back("luac");
}

bool LuaScriptResourceFormatLoader::handles_type(const String &p_type) const {
	print_debug("LuaScriptResourceFormatLoader::handles_type( p_type = " + p_type + " )");

	return (p_type == SCRIPT_TYPE || p_type == LUA_TYPE);
}

String LuaScriptResourceFormatLoader::get_resource_type(const String &p_path) const {
	print_debug("LuaScriptResourceFormatLoader::get_resource_type( p_path = " + p_path + " )");
	String el = p_path.get_extension().to_lower();
	if (el == LUA_EXTENSION )//luajit || el == LUAJIT_EXTENSION )
		return LUA_TYPE;
	return EMPTY_STRING;
}

Error LuaScriptResourceFormatSaver::save(const String &p_path, const Ref<Resource> &p_resource, uint32_t p_flags) {
	print_debug("LuaScriptResourceFormatSaver::save( p_path = " + p_path + " )");

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
	print_debug("LuaScriptResourceFormatSaver::get_recognized_extensions");

	if (Object::cast_to<LuaScript>(*p_resource)) {
		p_extensions->push_back(LUA_EXTENSION);
		//TODO:: luajit 
		//p_extensions->push_back(LUAJIT_EXTENSION);
	}
}

bool LuaScriptResourceFormatSaver::recognize(const Ref<Resource> &p_resource) const {
	print_debug("LuaScriptResourceFormatSaver::recognize");

	return Object::cast_to<LuaScript>(*p_resource) != nullptr;
}
