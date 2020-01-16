#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/script_language.h"
#include "debug.h"

class LuaScriptNativeClass : public Reference {

	GDCLASS(LuaScriptNativeClass, Reference);

	StringName name;

protected:
	bool _get(const StringName &p_name, Variant &r_ret) const;
	static void _bind_methods();

public:
	_FORCE_INLINE_ const StringName &get_name() const { return name; }
	Variant _new();
	Object *instance();
	LuaScriptNativeClass(const StringName &p_name);
};

class LuaScript : public Script {
	GDCLASS(LuaScript, Script)
	bool tool;
	bool valid;
	Ref<LuaScript> base;
	LuaScript *_base;
	//--unknown
	Ref<LuaScriptNativeClass> native;
	Set<Object *> instances;
	//--unknown
	String path;
	String source;
	const ClassDB::ClassInfo *cls;
	int lua_ref;
	Set<StringName> methods_name;
	Map<StringName, Variant> properties_default_value;
	Map<StringName, int> lua_properties_type;
	friend class LuaBindingHelper;
	friend class LuaScriptInstance;
	friend class LuaScriptLanguage;

public:
	LuaScript();
	~LuaScript();

private:
	//new function should bind to this
	ScriptInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_isref, Variant::CallError &r_error);

protected:

public:
	virtual bool can_instance() const;
	virtual Ref<Script> get_base_script() const {
		return base;
	}
	virtual StringName get_instance_base_type() const;
	/*TODO*/ virtual PlaceHolderScriptInstance *placeholder_instance_create(Object *p_this) {
		print_debug("LuaScript::placeholder_instance_create");
		return NULL;
	}
	virtual ScriptInstance *instance_create(Object *p_this);
	/*TODO*/ virtual bool instance_has(const Object *p_this) const {
		print_debug("LuaScript::instance_has");
		return false;
	}
	/*TODO*/ virtual bool has_source_code() const {
		print_debug("LuaScript::has_source_code");
		return false;
	}
	virtual String get_source_code() const;
	virtual void set_source_code(const String &p_code);
	virtual Error reload(bool p_keep_state = false);
	void add_lua_method(const StringName &method_name);
	virtual bool has_method(const StringName &p_method) const;
	// bool LuaScript::has_method_with_cls(const String &p_method) const;
	/*TODO*/ virtual MethodInfo get_method_info(const StringName &p_method) const {
		print_debug("LuaScript::get_method_info");
		return MethodInfo();
	}
	/*TODO*/ virtual bool is_tool() const {
		print_debug("LuaScript::is_tool");
		return tool;
	}
	virtual bool is_valid() const {
		print_debug("LuaScript::is_valid");
		return valid;
	}
	virtual ScriptLanguage *get_language() const;
	/*TODO*/ virtual bool has_script_signal(const StringName &p_signal) const {
		print_debug("LuaScript::has_script_signal");
		return false;
	}
	/*TODO*/ virtual void get_script_signal_list(List<MethodInfo> *r_signals) const {
		print_debug("LuaScript::get_script_signal_list");
	}
	void add_property_default_value(const StringName &p_property, const Variant &p_value);
	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const;
	/*TODO*/ virtual void get_script_method_list(List<MethodInfo> *p_list) const {
		print_debug("LuaScript::get_script_method_list");
	}
	/*TODO*/ virtual void get_script_property_list(List<PropertyInfo> *p_list) const {
		print_debug("LuaScript::get_script_property_list");
	}
	void add_lua_property_type(const StringName &name, int idx);
	Error load_source_code(const String &p_path);
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

#endif // LUASCRIPT_H
