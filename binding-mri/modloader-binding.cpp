//https://silverhammermba.github.io/emberb/c/
//https://docs.ruby-lang.org/capi/en/master/d8/d68/include_2ruby_2internal_2intern_2string_8h.html
#include "binding-util.h"
#include "sharedstate.h"
#include "filesystem.h"
#include "util.h"
#include "debugwriter.h"
#include "ruby/encoding.h"
#include "ruby/intern.h"
#include "ruby/thread.h"
#include "modloader.h"
#include <ruby.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>
#include <vector>
#include <string>

namespace fs = std::filesystem;

VALUE meow(std::vector<std::string> vec){
    VALUE ary = rb_ary_new_capa((long)vec.size());
    for (const std::string &s : vec) {
        VALUE str = rb_str_new_cstr(s.c_str());
        rb_ary_push(ary, str);
    }
    return ary;
}


static VALUE hooks(int argc, VALUE *argv, VALUE self) {
    VALUE v_path = Qnil;
    rb_scan_args(argc, argv, "01", &v_path);
    std::string hook_path = "hooks";
    if (v_path != Qnil) {
        v_path = StringValue(v_path);
        hook_path = StringValueCStr(v_path);
    }

	std::vector<std::string> files = {};
	if(!std::filesystem::exists(hook_path)){
		return meow(files);
	}
	for (const auto &entry : std::filesystem::directory_iterator(hook_path, std::filesystem::directory_options::skip_permission_denied)) {
		std::error_code ec;
		auto p = entry.path();
		if (!std::filesystem::is_regular_file(p, ec) || ec) continue;
		
		auto ext = p.extension().string();
		if (ext != ".rb") continue;
		
		std::string full = p.string();
		Debug() << "[MODLOADER] Executing hook: " << full;
		files.push_back(full);
	}
	return meow(files);
}

void ModLoaderBindingInit(){
	Debug() << "[MODLOADER] initalizing binding...";
	VALUE klass = rb_define_module("ModLoader");
	rb_define_module_function(klass, "hooks", RUBY_METHOD_FUNC(hooks), -1);
	rb_define_const(klass, "IS_ENABLED", modloader_is_enabled ? Qtrue : Qfalse);
	rb_define_const(klass, "COUNT", INT2NUM(mods_count));
}
