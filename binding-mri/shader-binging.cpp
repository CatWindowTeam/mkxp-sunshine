#include <ruby.h>
#include "render/effects.h"

VALUE rb_mShader;

void shaderBindingInit(){
    rb_mShader = rb_define_module("Shader");
    
    #define DEFINE_RUBY_CONST(name, rb) \
	rb_define_const(rb_mShader, #rb, INT2NUM(ShaderType::SHADER_##name));
        SHADER_LIST(DEFINE_RUBY_CONST)
    #undef DEFINE_RUBY_CONST
}
