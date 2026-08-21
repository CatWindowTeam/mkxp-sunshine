#include <physfs.h>
#include "binding-util.h"
#include "sharedstate.h"
#include "filesystem.h"
#include "util.h"
#include <ruby.h>
#include <stdexcept>
#include <string>
VALUE module = Qnil;
VALUE rb_ePhysFSError = Qnil;

std::string toStdString(VALUE v) {
	Check_Type(v, T_STRING);
	return std::string{ RSTRING_PTR(v), static_cast<std::size_t>(RSTRING_LEN(v)) };
}

static VALUE rb_PhysFS_Exist(VALUE, VALUE path) {
	if(PHYSFS_exists(StringValuePtr(path)) > 0){
		return Qtrue;
	}else{
		return Qfalse;
	}
}

static VALUE rb_PhysFS_Directory(VALUE, VALUE path) {
	if(PHYSFS_isDirectory(StringValuePtr(path)) > 0){
		return Qtrue;
	}else{
		return Qfalse;
	}
}

std::vector<char> loadFully(const char* path) {
	PHYSFS_File* file = PHYSFS_openRead(path);
	if (!file){
		WarnMsg("Failed to load PhysFS file %s", path);
		return {};
	}
	const auto length = PHYSFS_fileLength(file);
	std::vector<char> buffer;
	if (length > 0) {
		buffer.resize(static_cast<std::size_t>(length));
		const auto got = PHYSFS_readBytes(file, buffer.data(), static_cast<PHYSFS_uint64>(length));
		if (got != length && !PHYSFS_eof(file)) {
			PHYSFS_close(file);
			WarnMsg("PhysFS readBytes failed for '%s'", path);
		}
		buffer.resize(static_cast<std::size_t>(got));
	}
	PHYSFS_close(file);
	return buffer;
}

static VALUE rb_PhysFS_Read(VALUE, VALUE path) {
	try {
		const auto buf = loadFully(StringValuePtr(path));
		return rb_str_new(buf.data(), static_cast<long>(buf.size()));
	} catch (const std::exception& e) {
		rb_raise(rb_ePhysFSError, "%s", e.what());
	}
	return Qnil;
}

void PhysFS_binding_init(){
	VALUE module = rb_define_module("PhysFS");
	rb_ePhysFSError = rb_define_class_under(module, "Error", rb_eStandardError);
	rb_define_module_function(module, "exist?", rb_PhysFS_Exist, 1);
	rb_define_module_function(module, "directory?", rb_PhysFS_Directory, 1);
	rb_define_module_function(module, "read", rb_PhysFS_Read, 1);
}
