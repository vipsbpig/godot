#ifndef LUASCRIPT_INSTANCE_H
#define LUASCRIPT_INSTANCE_H

#include "luascript.h"


class LuaScriptInstance: public ScriptInstance
{
    friend class LuaScript;
private:
    Object *owner;
    Ref<LuaScript> script;

public:
    LuaScriptInstance();
    ~LuaScriptInstance();

    // ScriptInstance interface
public:
    /*TODO*/ virtual bool set(const StringName &p_name, const Variant &p_value){return false;}
    /*TODO*/ virtual bool get(const StringName &p_name, Variant &r_ret) const{return false;}
    /*TODO*/ virtual void get_property_list(List<PropertyInfo> *p_properties) const {}
    /*TODO*/ virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid) const {return Variant::NIL;}

    /*TODO*/ virtual Object *get_owner() {return owner;}
    /*TODO*/ virtual void get_property_state(List<Pair<StringName, Variant> > &state) {}

    /*TODO*/ virtual void get_method_list(List<MethodInfo> *p_list) const {}
    /*TODO*/ virtual bool has_method(const StringName &p_method) const {return false;}
    /*TODO*/ virtual Variant call(const StringName &p_method, VARIANT_ARG_LIST) {return Variant::NIL;}
    /*TODO*/ virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {return Variant::NIL;}
    /*TODO*/ virtual void call_multilevel(const StringName &p_method, VARIANT_ARG_LIST) {}
    /*TODO*/ virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount) {}
    /*TODO*/ virtual void call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount) {}

    /*TODO*/ virtual void notification(int p_notification) {}

    //this is used by script languages that keep a reference counter of their own
    //you can make make Ref<> not die when it reaches zero, so deleting the reference
    //depends entirely from the script
    /*TODO*/ virtual void refcount_incremented(){}
    /*TODO*/ virtual bool refcount_decremented(){return true;}

    virtual Ref<Script> get_script() const;

    /*TODO*/ virtual bool is_placeholder() const {return false;}

    /*TODO*/ virtual MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const{return MultiplayerAPI::RPCMode::RPC_MODE_DISABLED;}
    /*TODO*/ virtual MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const{return MultiplayerAPI::RPCMode::RPC_MODE_DISABLED;}

    virtual ScriptLanguage *get_language();
};

#endif // LUASCRIPT_INSTANCE_H
