#include <ruby.h>

#include "shader.h"

#include "disposable-binding.h"
#include "binding-util.h"
#include "binding-types.h"

VALUE rb_mShader;

void shaderBindingInit(){
	printf("[shaderBindingInit] Initializing Shaders binding\n");
    rb_mShader = rb_define_module("Shader");
    
    #define DEFINE_RUBY_CONST(name, type, rb) \
	rb_define_const(rb_mShader, #rb, INT2NUM(ShaderType::SHADER_##name));
        SHADER_LIST(DEFINE_RUBY_CONST)
    #undef DEFINE_RUBY_CONST
}