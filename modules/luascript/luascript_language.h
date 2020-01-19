#ifndef LUASCRIPT_LANGUAGE_H
#define LUASCRIPT_LANGUAGE_H

#include "core/script_language.h"
#include "lib/luabinding_helper.h"

class LuaScriptLanguage : public ScriptLanguage {
	static LuaScriptLanguage *singleton;
	LuaBindingHelper *binding;

	friend class LuaScript;
	friend class LuaScriptInstance;

public:
	LuaScriptLanguage();
	~LuaScriptLanguage();
	// ScriptLanguage interface
public:
	_FORCE_INLINE_ static LuaScriptLanguage *get_singleton() { return singleton; }
	_FORCE_INLINE_ LuaBindingHelper *get_binding() { return binding; }

	/* LANGUAGE FUNCTIONS */
	virtual String get_name() const;
	virtual void init();
	virtual String get_type() const { return "LuaScript"; }
	virtual String get_extension() const;
	virtual Error execute_file(const String &p_path);
	virtual void finish();

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const;
	virtual void get_string_delimiters(List<String> *p_delimiters) const;
	virtual Ref<Script> get_template(const String &p_class_name, const String &p_base_class_name) const;
	/*TODO*/ virtual void make_template(const String &p_class_name, const String &p_base_class_name, Ref<Script> &p_script) {}
	/*TODO*/ virtual bool is_using_templates() { return false; }
	/*TODO*/ virtual bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path, List<String> *r_functions, List<Warning> *r_warnings, Set<int> *r_safe_lines) const { return true; }
	/*TODO*/ virtual String validate_path(const String &p_path) const { return ""; }
	/*TODO*/ virtual Script *create_script() const { return NULL; }
	/*TODO*/ virtual bool has_named_classes() const { return false; }
	/*TODO*/ virtual bool supports_builtin_mode() const { return false; }
	/*TODO*/ virtual bool can_inherit_from_file() const { return false; }

	/*TODO*/ virtual int find_function(const String &p_function, const String &p_code) const { return 0; }
	/*TODO*/ virtual String make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const { return ""; }

	/*TODO*/ virtual Error open_in_external_editor(const Ref<Script> &p_script, int p_line, int p_col) { return ERR_UNAVAILABLE; }
	/*TODO*/ virtual bool overrides_external_editor() { return false; }

	/*TODO*/ virtual Error complete_code(const String &p_code, const String &p_base_path, Object *p_owner, List<String> *r_options, bool &r_force, String &r_call_hint) { return ERR_UNAVAILABLE; }
	/*TODO*/ virtual Error lookup_code(const String &p_code, const String &p_symbol, const String &p_base_path, Object *p_owner, LookupResult &r_result) { return ERR_UNAVAILABLE; }
	/*TODO*/ virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const {}
	/*TODO*/ virtual void add_global_constant(const StringName &p_variable, const Variant &p_value) {}
	/*TODO*/ virtual void add_named_global_constant(const StringName &p_name, const Variant &p_value) {}
	/*TODO*/ virtual void remove_named_global_constant(const StringName &p_name) {}

	/* MULTITHREAD FUNCTIONS */
	/*TODO*/ virtual void thread_enter() {}
	/*TODO*/ virtual void thread_exit() {}

	/* DEBUGGER FUNCTIONS */

	/*TODO*/ virtual String debug_get_error() const { return ""; }
	/*TODO*/ virtual int debug_get_stack_level_count() const { return -1; }
	/*TODO*/ virtual int debug_get_stack_level_line(int p_level) const { return -1; }
	/*TODO*/ virtual String debug_get_stack_level_function(int p_level) const { return ""; }
	/*TODO*/ virtual String debug_get_stack_level_source(int p_level) const { return ""; }
	/*TODO*/ virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {}
	/*TODO*/ virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {}
	/*TODO*/ virtual ScriptInstance *debug_get_stack_level_instance(int p_level) { return NULL; }
	/*TODO*/ virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {}
	/*TODO*/ virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems, int p_max_depth) { return ""; }
	/*TODO*/ virtual Vector<StackInfo> debug_get_current_stack_info() { return Vector<StackInfo>(); }
	/*TODO*/ virtual void reload_all_scripts() {}
	/*TODO*/ virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) {}

	/* LOADER FUNCTIONS */
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	/*TODO*/ virtual void get_public_functions(List<MethodInfo> *p_functions) const {}
	/*TODO*/ virtual void get_public_constants(List<Pair<String, Variant> > *p_constants) const {}
	/*TODO*/ virtual void profiling_start() {}
	/*TODO*/ virtual void profiling_stop() {}
	/*TODO*/ virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) { return -1; }
	/*TODO*/ virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) { return -1; }

	/* EXTRA FUNTIONS */
	/*TODO*/ virtual void *alloc_instance_binding_data(Object *p_object) { return NULL; }
	/*TODO*/ virtual void free_instance_binding_data(void *p_data) {}
	virtual void refcount_incremented_instance_binding(Object *p_object);
	virtual bool refcount_decremented_instance_binding(Object *p_object);
	/*TODO*/ virtual void frame() {}
	/*TODO*/ virtual bool handles_global_class_type(const String &p_type) const { return false; }
	/*TODO*/ virtual String get_global_class_name(const String &p_path, String *r_base_type, String *r_icon_path) const { return ""; }
};
#endif // LUASCRIPT_LANGUAGE_H
