#include "etc.h"
#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"
#include "sharedstate.h"

#include "signalconnection-binding.h"

void RubyConnection::Disconnect(){
    RubyConnection::connection.Disconnect();
}
bool RubyConnection::Connected(){
    return RubyConnection::connection.Connected();
}

VALUE rb_cRubyConnection = Qnil;

void rubyConnection_free(void* ptr)
{
    RubyConnection* conn = static_cast<RubyConnection*>(ptr);
    conn->Disconnect();
    delete conn;
}

void rubyConnection_mark(void *ptr)
{
    RubyConnection *conn = static_cast<RubyConnection*>(ptr);

    if (!NIL_P(conn->proc))
        rb_gc_mark(conn->proc);
}

const rb_data_type_t rubyConnection_type = { "RubyConnection", {rubyConnection_mark, rubyConnection_free, 0}, 0, 0, RUBY_TYPED_FREE_IMMEDIATELY };

static VALUE rubyConnection_alloc(VALUE klass)
{
    RubyConnection* ptr = ALLOC(RubyConnection);
    return TypedData_Wrap_Struct(klass, &rubyConnection_type, ptr);
}

static VALUE rubyConnectionDisconnect(VALUE self){
    RubyConnection* conn;
    TypedData_Get_Struct(self, RubyConnection, &rubyConnection_type, conn);
    conn->Disconnect();
    return Qnil;
}

static VALUE rubyConnectionConnected(VALUE self){
    RubyConnection* conn;
    TypedData_Get_Struct(self, RubyConnection, &rubyConnection_type, conn);
    return rb_bool_new(conn->Connected());
}

void SignalConnectionBindingInit(){
    rb_cRubyConnection = rb_define_class("RubyConnection", rb_cObject);
    rb_define_alloc_func(rb_cRubyConnection, rubyConnection_alloc);
    rb_define_method(rb_cRubyConnection, "disconnect", RUBY_METHOD_FUNC(rubyConnectionDisconnect), 0);
    rb_define_method(rb_cRubyConnection, "connected",  RUBY_METHOD_FUNC(rubyConnectionConnected),  0);
}