#pragma once

#include "signals/signalconnection.h"

#include <ruby.h>

extern void rubyConnection_free(void* ptr);
extern void rubyConnection_mark(void* ptr);
extern const rb_data_type_t rubyConnection_type;
extern VALUE rb_cRubyConnection;

struct RubyConnection
{
    VALUE proc;
    SignalConnection connection;

    void Disconnect();
    bool Connected();
};

#define RUBY_CONNECTION \
	if (!rb_block_given_p()) \
        rb_raise(rb_eArgError, "unable to connect signal, block required"); \
	VALUE proc = rb_block_proc(); \
	RubyConnection* conn = new RubyConnection(); \
	conn->proc = proc;
