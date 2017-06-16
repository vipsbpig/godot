def can_build(platform):
    return platform=="android"

def configure(env):
    if env['platform'] == 'android':
		env.android_add_java_dir(".")
		env.disable_module()
