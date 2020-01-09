#include "luascript_language.h"
#include "debug.h"
LuaScriptLanguage *LuaScriptLanguage::singleton = NULL;

LuaScriptLanguage::LuaScriptLanguage() {
	ERR_FAIL_COND(singleton);
	singleton = this;
	binding = memnew(LuaBindingHelper);
}

LuaScriptLanguage::~LuaScriptLanguage() {
	memdelete(binding);
}

String LuaScriptLanguage::get_name() const {
	return "LuaScript";
}
/* LANGUAGE FUNCTIONS */
void LuaScriptLanguage::init() {
	binding->initialize();
	execute_file("luadebug.lua");
}

String LuaScriptLanguage::get_extension() const {
	return "lua";
}

Error LuaScriptLanguage::execute_file(const String &p_path) {
	ERR_FAIL_NULL_V(binding, ERR_BUG);
	ERR_FAIL_NULL_V(binding, ERR_BUG);
	FileAccessRef f = FileAccess::open(p_path, FileAccess::READ);
	ERR_FAIL_COND_V(!f, ERR_INVALID_PARAMETER);

	Vector<uint8_t> buff;
	buff.resize(f->get_len() + 1);
	f->get_buffer(buff.ptrw(), f->get_len());
	buff.ptrw()[buff.size() - 1] = 0;

	String source;
	source.parse_utf8((const char *)buff.ptr(), buff.size());

	return binding->script(source);
}

void LuaScriptLanguage::finish() {
	binding->uninitialize();
}
/* EDITOR FUNCTIONS */
void LuaScriptLanguage::get_reserved_words(List<String> *p_words) const {
	static const char *_reserved_words[] = {
		//lua keyword
		"and",
		"break",
		"do",
		"else",
		"elseif",
		"end"
		"false",
		"for",
		"function",
		"if"
		"in",
		"local"
		"nil",
		"not",
		"or",
		"repeat",
		"return",
		"then",
		"true",
		"until",
		"while",
		//godot extras
		"PI",
		"TAU",
		"INF",
		"NAN",
		"export",
		"extends",
	};

	const char **w = _reserved_words;

	while (*w) {

		p_words->push_back(*w);
		w++;
	}

	//TODO::到时候需要绑定godot的数学库添加些基础功能
	//for (int i = 0; i < GDScriptFunctions::FUNC_MAX; i++) {
	//	p_words->push_back(GDScriptFunctions::get_func_name(GDScriptFunctions::Function(i)));
	//}
}

void LuaScriptLanguage::get_comment_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("--[[ ]]"); // Block comment starts with double shovel and runs until double close box
	p_delimiters->push_back("--"); // Single-line comment starts with a double hyphens
}

void LuaScriptLanguage::get_string_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("\" \"");
	p_delimiters->push_back("' '");
	p_delimiters->push_back("[[ ]]"); // Mult-line strings
}

void LuaScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("lua");
}
