def is_enabled():
    # The module is disabled by default. Use module_luascript_enabled=yes to enable it.
    return True

def can_build(env, platform):
    return True

def configure(env):
    #pass
    bits = env['bits']
    lua_include_path = "#modules/luascript/lib/luajit/src"

    if env['platform'] == 'windows':
        env.Append(LIBPATH=['#modules/luascript/lib/luajit'])
        if bits == '32':
            env.Append(LINKFLAGS=['lua51_x86.lib'])
        else:
            env.Append(LINKFLAGS=['lua51_x64.lib'])




def get_doc_classes():
	return ["LuaScript"]

def get_doc_path():
	return "doc_classes"
