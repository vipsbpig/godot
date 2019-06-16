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

class LuaScript : public Script
{
    GDCLASS(LuaScript, Script)
    bool tool;
    bool valid;

    friend class LuaScriptInstance;
    friend class LuaScriptLanguage;
public:
    LuaScript();
    ~LuaScript();

    // Script interface
public:
    virtual bool can_instance() const;
    /*TODO*/ virtual Ref<Script> get_base_script() const { return NULL;}
    /*TODO*/ virtual StringName get_instance_base_type() const {return StringName();}
    /*TODO*/ virtual PlaceHolderScriptInstance *placeholder_instance_create(Object *p_this){return NULL;}
    /*TODO*/ virtual ScriptInstance *instance_create(Object *p_this) {return NULL;}
    /*TODO*/ virtual bool instance_has(const Object *p_this) const {return false;}
    /*TODO*/ virtual bool has_source_code() const {return false;}
    /*TODO*/ virtual String get_source_code() const {return "";}
    /*TODO*/ virtual void set_source_code(const String &p_code) {}
    /*TODO*/ virtual Error reload(bool p_keep_state = false) {return ERR_UNAVAILABLE;}
    /*TODO*/ virtual bool has_method(const StringName &p_method) const {return false;}
    /*TODO*/ virtual MethodInfo get_method_info(const StringName &p_method) const {return MethodInfo();}
    /*TODO*/ virtual bool is_tool() const {return tool;}
    /*TODO*/ virtual bool is_valid() const {return valid;}
    virtual ScriptLanguage *get_language() const;
    /*TODO*/ virtual bool has_script_signal(const StringName &p_signal) const {return false;}
    /*TODO*/ virtual void get_script_signal_list(List<MethodInfo> *r_signals) const {}
    /*TODO*/ virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const {return false;}
    /*TODO*/ virtual void get_script_method_list(List<MethodInfo> *p_list) const {}
    /*TODO*/ virtual void get_script_property_list(List<PropertyInfo> *p_list) const {}

    /*TODO*/ Error load_source_code(const String &p_path){return ERR_UNAVAILABLE;}

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
