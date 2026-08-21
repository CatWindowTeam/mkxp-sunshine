//https://silverhammermba.github.io/emberb/c/
//https://docs.ruby-lang.org/capi/en/master/d8/d68/include_2ruby_2internal_2intern_2string_8h.html
#include "binding-util.h"
#include "util.h"
#include "debugwriter.h"
#include "modloader.h"
#include "config.h"
#include <ruby.h>
#include <physfs.h>
#include <string>
#include <vector>
VALUE meow(const std::vector<std::string> vec){
    VALUE ary = rb_ary_new_capa((long)vec.size());
    for (const std::string &s : vec) {
        VALUE str = rb_str_new_cstr(s.c_str());
        rb_ary_push(ary, str);
    }
    return ary;
}

static VALUE hooks(int argc, VALUE *argv, VALUE self){
    VALUE v_path = Qnil;
    rb_scan_args(argc, argv, "01", &v_path);
    std::string hook_path = "hooks";
    if (v_path != Qnil) {
        v_path = StringValue(v_path);
        hook_path = StringValueCStr(v_path);
    }

    std::vector<std::string> files;
    if (!PHYSFS_exists(hook_path.c_str())) {
        return meow(files);
    }

    PHYSFS_Stat dir_stat{};

    if (
        PHYSFS_stat(hook_path.c_str(), &dir_stat) == 0 ||
        dir_stat.filetype != PHYSFS_FILETYPE_DIRECTORY
    ) {
        return meow(files);
    }

    char **list = PHYSFS_enumerateFiles(hook_path.c_str());

    if (list == nullptr) {
        return meow(files);
    }

    for (char **entry = list; *entry != nullptr; ++entry) {
        std::string name = *entry;
        std::string full_path = hook_path;

        if (!full_path.empty() && full_path.back() != '/') {
            full_path += '/';
        }

        full_path += name;
        PHYSFS_Stat file_stat{};
        if (PHYSFS_stat(full_path.c_str(), &file_stat) == 0) {
            continue;
        }

        if (file_stat.filetype != PHYSFS_FILETYPE_REGULAR) {
            continue;
        }

        if (name.size() < 3 ||
            name.compare(name.size() - 3, 3, ".rb") != 0) {
            continue;
        }

        Debug() << "[MODLOADER] Executing hook: " << full_path;
        files.push_back(full_path);
    }
    PHYSFS_freeList(list);
    return meow(files);
}

void ModLoaderBindingInit(){
	VALUE klass = rb_define_module("ModLoader");
	rb_define_module_function(klass, "hooks", RUBY_METHOD_FUNC(hooks), -1);
	rb_define_const(klass, "IS_ENABLED", modloader_is_enabled ? Qtrue : Qfalse);
	rb_define_const(klass, "COUNT", INT2NUM(mods_count));
	rb_define_const(klass, "PRELOAD_SCRIPTS_COUNT", INT2NUM(preloaded_script_count));
}
