#include "binding.h"
#include "binding-util.h"
#include "debugwriter.h"
#include <ruby.h>
#include <ruby/debug.h>
#include <inttypes.h>
#include <time.h>
#include <string>
#include <vector>
VALUE tp = Qnil;
static std::vector<uint64_t> call_stack;  // Глобальный стек вместо thread-local

static uint64_t now_ns(void) {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void tp_cb(VALUE tpval, void *) {
    rb_trace_arg_t *trace_arg = rb_tracearg_from_tracepoint(tpval);
    if (!trace_arg) return;

    rb_event_flag_t ev = rb_tracearg_event_flag(trace_arg);

    if (ev == RUBY_EVENT_CALL) {
        call_stack.push_back(now_ns());
        return;
    }

    if (ev == RUBY_EVENT_RETURN) {
        if (call_stack.empty()) return;

        uint64_t end_ns = now_ns();
        uint64_t start_ns = call_stack.back();
        call_stack.pop_back();
        uint64_t dur_ns = end_ns - start_ns;

        // Сохраняем VALUE в переменные ПЕРЕД StringValuePtr
        VALUE path_val = rb_tracearg_path(trace_arg);
        VALUE class_val = rb_tracearg_defined_class(trace_arg);
        
        const char *path = StringValuePtr(path_val);
        const char *class_name = StringValuePtr(class_val);
        long line = FIX2LONG(rb_tracearg_lineno(trace_arg));
        const char *method = rb_id2name((ID)rb_tracearg_method_id(trace_arg));

        Debug() << "[PROFILER] " << method << " " << path << ":" << line
                << " class=" << class_name << " dur_ns=" << dur_ns << "\n";
    }
}


static VALUE profiler_set(VALUE, VALUE v) {
    if (NIL_P(tp)) {
        rb_raise(rb_eRuntimeError, "Profiler not initialized");
        return Qnil;
    }
    
    if (v == Qtrue) {
        rb_tracepoint_enable(tp);
    } else {
        rb_tracepoint_disable(tp);
    }
    return Qnil;
}

void ProfilerInit() {
    VALUE module = rb_define_module("Profiler");
    tp = rb_tracepoint_new(Qnil, RUBY_EVENT_CALL | RUBY_EVENT_RETURN, tp_cb, nullptr);
    
    rb_gc_register_address(&tp);
    rb_define_singleton_method(module, "set", RUBY_METHOD_FUNC(profiler_set), 1);
}
