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

#pragma once

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/mutex.h"
#include "core/script_language.h"

#include "sol.hpp"

class LuaScriptInstance;

class LuaNativeClass : public Reference {

	GDCLASS(LuaNativeClass, Reference);

	StringName name;

protected:
	bool _get(const StringName &p_name, Variant &r_ret) const;
	static void _bind_methods();

public:
	_FORCE_INLINE_ const StringName &get_name() const { return name; }
	Variant _new();
	Object *instance();
	LuaNativeClass(const StringName &p_name);
};

class LuaScript : public Script {

	GDCLASS(LuaScript, Script)
	bool tool;
	bool valid;

	friend class LuaScriptInstance;
	friend class LuaScriptLanguage;

	//Variant _static_ref; //used for static call
	Ref<LuaNativeClass> native;
	Ref<LuaScript> base;
	LuaScript *_base; //fast pointer access
	//LuaScript *_owner; //for subclasses

	//Set<StringName> members; //members are just indices to the instanced script.
	//Map<StringName, Variant> constants;
	//Map<StringName, GDScriptFunction *> member_functions;
	//Map<StringName, MemberInfo> member_indices; //members are just indices to the instanced script.
	//Map<StringName, Ref<GDScript> > subclasses;
	//Map<StringName, Vector<StringName> > _signals;

	//Unknown

#ifdef TOOLS_ENABLED
	bool source_changed_cache;
	Map<StringName, Variant> member_default_values;

#endif
	Map<StringName, PropertyInfo> member_info;
		
	Set<Object *> instances;
	//exported members
	String source;
	Vector<uint8_t> bytecode;
	String path;
	String name;
	int ref; // ref to loaded lua script chunk(function)

    /*TODO*/ LuaScriptInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_isref);

#ifdef TOOLS_ENABLED
	Set<PlaceHolderScriptInstance *> placeholders;
    /*TODO*/ virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder);
    /*TODO*/ void _update_exports_values(Map<StringName, Variant> &values, List<PropertyInfo> &propnames);

#endif

	//#ifdef DEBUG_ENABLED
	//	Map<ObjectID, List<Pair<StringName, Variant> > > pending_reload_state;
	//#endif

	//bool _update_exports();

protected:
    /*TODO*/ bool _get(const StringName &p_name, Variant &r_ret) const;
    /*TODO*/ bool _set(const StringName &p_name, const Variant &p_value);
    /*TODO*/ void _get_property_list(List<PropertyInfo> *p_properties) const;

    /*TODO*/ Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	//	void call_multilevel(const StringName& p_method,const Variant** p_args,int p_argcount);

    /*TODO*/ static void _bind_methods();

public:
	virtual bool is_valid() const { return valid; }
	virtual bool is_tool() const { return tool; }

    /*TODO*/ Ref<LuaScript> get_base() const;

    /*TODO*/ virtual bool can_instance() const;

    /*TODO*/ virtual Ref<Script> get_base_script() const;

    /*TODO*/ virtual StringName get_instance_base_type() const;
    /*TODO*/ virtual ScriptInstance *instance_create(Object *p_this);
    /*TODO*/ virtual bool instance_has(const Object *p_this) const;

    /*TODO*/ virtual bool has_source_code() const;
    /*TODO*/ virtual String get_source_code() const;
    /*TODO*/ virtual void set_source_code(const String &p_code);
    /*TODO*/ virtual Error reload(bool p_keep_state = false);

    /*TODO*/ virtual bool has_method(const StringName &p_method) const;
    /*TODO*/ virtual MethodInfo get_method_info(const StringName &p_method) const;


    /*TODO*/ virtual ScriptLanguage *get_language() const;

    /*TODO*/ virtual bool has_script_signal(const StringName &p_signal) const;
    /*TODO*/ virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;

    /*TODO*/ virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const;

    /*TODO*/ virtual void update_exports();

    /*TODO*/ virtual void get_script_method_list(List<MethodInfo> *p_list) const;
    /*TODO*/ virtual void get_script_property_list(List<PropertyInfo> *p_list) const;

    /*TODO*/ virtual int get_member_line(const StringName &p_member) const;

    /*TODO*/ virtual void get_constants(Map<StringName, Variant> *p_constants);
    /*TODO*/ virtual void get_members(Set<StringName> *p_constants);

    /*TODO*/ Error load_byte_code(const String &p_path);
    /*TODO*/ Error load_source_code(const String &p_path);

    /*TODO*/ Variant _new(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

    /*TODO*/ LuaScript();
    /*TODO*/ ~LuaScript();
};

class LuaScriptInstance : public ScriptInstance {

	friend class LuaScript;

private:
	Object *owner;
	Ref<LuaScript> script;

public:
	LuaScriptInstance();
	~LuaScriptInstance();

    /*TODO*/ virtual bool set(const StringName &p_name, const Variant &p_value);
    /*TODO*/ virtual bool get(const StringName &p_name, Variant &r_ret) const;
    /*TODO*/ virtual void get_property_list(List<PropertyInfo> *p_properties) const;
    /*TODO*/ virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = NULL) const;

    /*TODO*/ virtual Object *get_owner();

    /*TODO*/ virtual void get_method_list(List<MethodInfo> *p_list) const;
    /*TODO*/ virtual bool has_method(const StringName &p_method) const;
    /*TODO*/ virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
    /*TODO*/ virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount);
    /*TODO*/ virtual void call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount);
    /*TODO*/ virtual void notification(int p_notification);

    /*TODO*/ virtual void refcount_incremented();
    /*TODO*/ virtual bool refcount_decremented();

    /*TODO*/ virtual Ref<Script> get_script() const;

    /*TODO*/ virtual MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const;
    /*TODO*/ virtual MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const;

    /*TODO*/ virtual ScriptLanguage *get_language();

    static void tmpLuaDebug(const char *fname);
    /*TODO*/ static int setup();
};

class LuaScriptLanguage : public ScriptLanguage {

	static LuaScriptLanguage *singleton ;

	friend class LuaScript;
	friend class LuaScriptInstance;

	Variant* _global_array;
	Vector<Variant> global_array;
	Map<StringName, int> globals;

	void _add_global(const StringName &p_name, const Variant &p_value);

private:


private:
    //SOL2 state
    sol::state* lua;

public:
    //SOL2 state
    sol::state* get_state(){return lua;}

	_FORCE_INLINE_ static LuaScriptLanguage *get_singleton() { return singleton; }

	LuaScriptLanguage();
	~LuaScriptLanguage();

	virtual String get_name() const;

	virtual void init();
	virtual String get_type() const;
	virtual String get_extension() const;
	virtual Error execute_file(const String &p_path);
    /*TODO*/ virtual void finish();

	virtual void get_reserved_words(List<String> *p_words) const;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const;
	virtual void get_string_delimiters(List<String> *p_delimiters) const;
	virtual Ref<Script> get_template(const String &p_class_name, const String &p_base_class_name) const;
	virtual void make_template(const String &p_class_name, const String &p_base_class_name, Ref<Script> &p_script);
	virtual bool is_using_templates();
	virtual bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path = "", List<String> *r_functions = NULL, List<Warning> *r_warnings = NULL, Set<int> *r_safe_lines = NULL) const;
	virtual Script *create_script() const;
	virtual bool has_named_classes() const;
	virtual bool supports_builtin_mode() const;
	virtual bool can_inherit_from_file();
    /*TODO*/ virtual int find_function(const String &p_function, const String &p_code) const;
	virtual String make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const;
	//virtual Error open_in_external_editor(const Ref<Script> &p_script, int p_line, int p_col);
	//virtual bool overrides_external_editor();

    /*TODO*/ virtual Error complete_code(const String &p_code, const String &p_base_path, Object *p_owner, List<String> *r_options, bool &r_force, String &r_call_hint);
#ifdef TOOLS_ENABLED
    /*TODO*/ virtual Error lookup_code(const String &p_code, const String &p_symbol, const String &p_base_path, Object *p_owner, LookupResult &r_result);
#endif

    /*TODO*/ virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const;
	virtual void add_global_constant(const StringName &p_variable, const Variant &p_value);

    /*TODO*/ virtual void thread_enter();
    /*TODO*/ virtual void thread_exit();

    /*TODO*/ virtual String debug_get_error() const;
    /*TODO*/ virtual int debug_get_stack_level_count() const;
    /*TODO*/ virtual int debug_get_stack_level_line(int p_level) const;
    /*TODO*/ virtual String debug_get_stack_level_function(int p_level) const;
    /*TODO*/ virtual String debug_get_stack_level_source(int p_level) const;
    /*TODO*/ virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
    /*TODO*/ virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
    /*TODO*/ virtual ScriptInstance *debug_get_stack_level_instance(int p_level);
    /*TODO*/ virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
    /*TODO*/ virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1);


    /*TODO*/ virtual void reload_all_scripts();
    /*TODO*/ virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload);

	virtual void get_recognized_extensions(List<String> *p_extensions) const;
    /*TODO*/ virtual void get_public_functions(List<MethodInfo> *p_functions) const;
    /*TODO*/ virtual void get_public_constants(List<Pair<String, Variant> > *p_constants) const;

    /*TODO*/ virtual void profiling_start();
    /*TODO*/ virtual void profiling_stop();

    /*TODO*/ virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max);
    /*TODO*/ virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max);

    /*TODO*/ virtual void *alloc_instance_binding_data(Object *p_object);
    /*TODO*/ virtual void free_instance_binding_data(void *p_data);

    /*TODO*/ virtual void frame();

    /*TODO*/ bool debug_break_parse(const String &p_file, int p_line, const String &p_error);

private:
};

class LuaScriptResourceFormatLoader : public ResourceFormatLoader {
	GDCLASS(LuaScriptResourceFormatLoader, ResourceFormatLoader)
public:
    virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = NULL);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class LuaScriptResourceFormatSaver : public ResourceFormatSaver {
	GDCLASS(LuaScriptResourceFormatSaver, ResourceFormatSaver)
public:
	virtual Error save(const String &p_path, const Ref<Resource> &p_resource, uint32_t p_flags = 0);
    virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const;
	virtual bool recognize(const Ref<Resource> &p_resource) const;
};
