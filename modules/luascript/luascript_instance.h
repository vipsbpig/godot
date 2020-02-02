#ifndef LUASCRIPT_INSTANCE_H
#define LUASCRIPT_INSTANCE_H

#include "luascript.h"

class LuaScriptInstance : public ScriptInstance {
	friend class LuaScript;
	friend class LuaBindingHelper;

private:
	Object *owner;
	Ref<LuaScript> script;
	int lua_ref;

public:
	bool base_ref;

	LuaScriptInstance();
	~LuaScriptInstance();

	// ScriptInstance interface
public:
	Error initialize(const Variant **p_args, int p_argcount, bool p_ref = false);
	virtual bool set(const StringName &p_name, const Variant &p_value);
	virtual bool get(const StringName &p_name, Variant &r_ret) const;
	/*TODO*/ virtual void get_property_list(List<PropertyInfo> *p_properties) const;
	/*TODO*/ virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid) const;

	virtual Object *get_owner() { return owner; }
	///*TODO*/ virtual void get_property_state(List<Pair<StringName, Variant> > &state) {}

	/*TODO*/ virtual void get_method_list(List<MethodInfo> *p_list) const {}
	virtual bool has_method(const StringName &p_method) const;
	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	/*TODO*/ virtual void notification(int p_notification) {}

	virtual Ref<Script> get_script() const;

	///*TODO*/ virtual bool is_placeholder() const { return false; }

	/*TODO*/ virtual MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const { return MultiplayerAPI::RPCMode::RPC_MODE_DISABLED; }
	/*TODO*/ virtual MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const { return MultiplayerAPI::RPCMode::RPC_MODE_DISABLED; }

	virtual ScriptLanguage *get_language();
};

#endif // LUASCRIPT_INSTANCE_H
