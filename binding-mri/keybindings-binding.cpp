#include "input.h"
#include "sharedstate.h"
#include "binding-util.h"

#include "keybindings-binding.h"

void sourceDesc_free(void* ptr)
{
    SourceDesc* kb = (SourceDesc*)ptr;
    delete kb;
}

const rb_data_type_t sourceDesc_type = { "SourceDesc", {0, sourceDesc_free, 0}, 0, 0, RUBY_TYPED_FREE_IMMEDIATELY };

static VALUE sourceDesc_alloc(VALUE klass)
{
    SourceDesc* ptr = ALLOC(SourceDesc);

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, ptr);
}

// fabrics
static VALUE rb_source_key(VALUE klass, VALUE scan)
{
    SourceDesc* s = new SourceDesc();

    s->type = Key;
    s->d.scan = static_cast<SDL_Scancode>(NUM2INT(scan));

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

static VALUE rb_source_caxis(VALUE klass, VALUE axis, VALUE dir)
{
    SourceDesc* s = new SourceDesc();

    s->type = CAxis;
    s->d.ja.axis = NUM2INT(axis);
    s->d.ja.dir = static_cast<AxisDir>(NUM2INT(dir));

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

static VALUE rb_source_cbutton(VALUE klass, VALUE button)
{
    SourceDesc* s = new SourceDesc();

    s->type = CButton;
    s->d.jb = NUM2INT(button);

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

static VALUE rb_source_jaxis(VALUE klass, VALUE axis, VALUE dir)
{
    SourceDesc* s = new SourceDesc();

    s->type = JAxis;
    s->d.ja.axis = NUM2INT(axis);
    s->d.ja.dir = static_cast<AxisDir>(NUM2INT(dir));

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

static VALUE rb_source_jhat(VALUE klass, VALUE hat, VALUE pos)
{
    SourceDesc* s = new SourceDesc();

    s->type = JHat;
    s->d.jh.hat = NUM2INT(hat);
    s->d.jh.pos = NUM2INT(pos);

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

static VALUE rb_source_jbutton(VALUE klass, VALUE button)
{
    SourceDesc* s = new SourceDesc();

    s->type = JButton;
    s->d.jb = NUM2INT(button);

    return TypedData_Wrap_Struct(klass, &sourceDesc_type, s);
}

// getters
#define SOURCE_DESC \
    SourceDesc* src; \
    TypedData_Get_Struct(self, SourceDesc, &sourceDesc_type, src);

#define TYPE_IS src->type == SourceType

static VALUE rb_source_type(VALUE self)
{
    SOURCE_DESC
    return INT2NUM(src->type);
}

static VALUE rb_source_scancode(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::Key)
        return INT2NUM(src->d.scan);
    return Qnil;
}

static VALUE rb_source_button(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::CButton || TYPE_IS::JButton)
        return INT2NUM(src->d.jb);
    return Qnil;
}

static VALUE rb_source_axis(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::CAxis || TYPE_IS::JAxis)
        return INT2NUM(src->d.ja.axis);
    return Qnil;
}

static VALUE rb_source_dir(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::CAxis || TYPE_IS::JAxis)
        return INT2NUM(src->d.ja.dir);
    return Qnil;
}

static VALUE rb_source_hat(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::JHat)
        return INT2NUM(src->d.jh.hat);
    return Qnil;
}

static VALUE rb_source_pos(VALUE self)
{
    SOURCE_DESC
    if (TYPE_IS::JHat)
        return INT2NUM(src->d.jh.pos);
    return Qnil;
}

static VALUE rb_source_marshal_dump(VALUE self)
{
    SourceDesc* src;
    TypedData_Get_Struct(self, SourceDesc, &sourceDesc_type, src);

    VALUE arr = rb_ary_new();

    rb_ary_push(arr, INT2NUM(src->type));

    switch (src->type)
    {
        case SourceType::Key:
            rb_ary_push(arr, INT2NUM(src->d.scan));
            break;

        case SourceType::CButton:
        case SourceType::JButton:
            rb_ary_push(arr, INT2NUM(src->d.jb));
            break;

        case SourceType::CAxis:
        case SourceType::JAxis:
            rb_ary_push(arr, INT2NUM(src->d.ja.axis));
            rb_ary_push(arr, INT2NUM(src->d.ja.dir));
            break;

        case SourceType::JHat:
            rb_ary_push(arr, INT2NUM(src->d.jh.hat));
            rb_ary_push(arr, INT2NUM(src->d.jh.pos));
            break;

        default:
            break;
    }

    return arr;
}

static VALUE rb_source_marshal_load(VALUE self, VALUE data)
{
    SourceDesc* src;
    TypedData_Get_Struct(self, SourceDesc, &sourceDesc_type, src);

    Check_Type(data, T_ARRAY);

    VALUE type = rb_ary_entry(data, 0);
    src->type = static_cast<SourceType>(NUM2INT(type));

    switch (src->type)
    {
        case SourceType::Key:
            src->d.scan = static_cast<SDL_Scancode>(NUM2INT(rb_ary_entry(data, 1)));
            break;

        case SourceType::CButton:
        case SourceType::JButton:
            src->d.jb = NUM2INT(rb_ary_entry(data, 1));
            break;

        case SourceType::CAxis:
        case SourceType::JAxis:
            src->d.ja.axis = NUM2INT(rb_ary_entry(data, 1));
            src->d.ja.dir  = static_cast<AxisDir>(NUM2INT(rb_ary_entry(data, 2)));
            break;

        case SourceType::JHat:
            src->d.jh.hat = NUM2INT(rb_ary_entry(data, 1));
            src->d.jh.pos = NUM2INT(rb_ary_entry(data, 2));
            break;

        default:
            break;
    }

    return self;
}

void keybindingsBindingInit(){
	VALUE klass = rb_define_class("KeyBind", rb_cObject);
    rb_define_alloc_func(klass, sourceDesc_alloc);
    
    rb_define_singleton_method(klass, "key",     RUBY_METHOD_FUNC(rb_source_key), 1);
    rb_define_singleton_method(klass, "caxis",   RUBY_METHOD_FUNC(rb_source_caxis), 2);
    rb_define_singleton_method(klass, "cbutton", RUBY_METHOD_FUNC(rb_source_cbutton), 1);
    rb_define_singleton_method(klass, "jaxis",   RUBY_METHOD_FUNC(rb_source_jaxis), 2);
    rb_define_singleton_method(klass, "jhat",    RUBY_METHOD_FUNC(rb_source_jhat), 2);
    rb_define_singleton_method(klass, "jbutton", RUBY_METHOD_FUNC(rb_source_jbutton), 1);

    rb_define_method(klass, "type",     RUBY_METHOD_FUNC(rb_source_type), 0);
    rb_define_method(klass, "scancode", RUBY_METHOD_FUNC(rb_source_scancode), 0);
    rb_define_method(klass, "button",   RUBY_METHOD_FUNC(rb_source_button), 0);
    rb_define_method(klass, "axis",     RUBY_METHOD_FUNC(rb_source_axis), 0);
    rb_define_method(klass, "dir",      RUBY_METHOD_FUNC(rb_source_dir), 0);
    rb_define_method(klass, "hat",      RUBY_METHOD_FUNC(rb_source_hat), 0);
    rb_define_method(klass, "pos",      RUBY_METHOD_FUNC(rb_source_pos), 0);
    
    rb_define_method(klass, "marshal_dump", RUBY_METHOD_FUNC(rb_source_marshal_dump), 0);
    rb_define_method(klass, "marshal_load", RUBY_METHOD_FUNC(rb_source_marshal_load), 1);

    rb_const_set(klass, rb_intern("Negative"), INT2FIX(0));
    rb_const_set(klass, rb_intern("Positive"), INT2FIX(1));

    VALUE module = rb_define_module_under(klass, "Type");

    rb_const_set(module, rb_intern("Invalid"), INT2FIX(SourceType::Invalid));
    rb_const_set(module, rb_intern("Key"),     INT2FIX(SourceType::Key));
    rb_const_set(module, rb_intern("CButton"), INT2FIX(SourceType::CButton));
    rb_const_set(module, rb_intern("CAxis"),   INT2FIX(SourceType::CAxis));
    rb_const_set(module, rb_intern("JButton"), INT2FIX(SourceType::JButton));
    rb_const_set(module, rb_intern("JAxis"),   INT2FIX(SourceType::JAxis));
    rb_const_set(module, rb_intern("JHat"),    INT2FIX(SourceType::JHat));
}