//SOMEONE PLZ HELP ME FIX TIHIS SHITTTTTTTTTTT


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

static uint64_t now_ns(void) {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static std::string value_to_string(VALUE v, const char *fallback) {
    if (NIL_P(v)) return fallback;
    return std::string(StringValuePtr(v));
}

static void tp_cb(VALUE tpval, void *) {
    rb_trace_arg_t *trace_arg = rb_tracearg_from_tracepoint(tpval);
    if (!trace_arg) return;

    rb_event_flag_t ev = rb_tracearg_event_flag(trace_arg);
    struct Stack {
        std::vector<uint64_t> starts;
    };
    static __thread Stack stack;

    if (ev == RUBY_EVENT_CALL) {
        stack.starts.push_back(now_ns());
        return;
    }

    if (ev == RUBY_EVENT_RETURN) {
        if (stack.starts.empty()) return;

        uint64_t end_ns = now_ns();
        uint64_t start_ns = stack.starts.back();
        stack.starts.pop_back();

        uint64_t dur_ns = end_ns - start_ns;

        VALUE path = rb_tracearg_path(trace_arg);
        VALUE lineno = rb_tracearg_lineno(trace_arg);
        VALUE defined_class = rb_tracearg_defined_class(trace_arg);
        VALUE method_id = rb_tracearg_method_id(trace_arg);

        std::string path_s = value_to_string(path, "<nil>");
        std::string class_s = value_to_string(defined_class, "<nil>");

        std::string meth_s = "<nil>";
        if (!NIL_P(method_id)) {
            const char *mn = rb_id2name((ID)method_id);
            if (mn) meth_s = mn;
        }

        long line = NIL_P(lineno) ? 0 : FIX2LONG(lineno);

        Debug() << "[PROFILER] "
                << meth_s << ' '
                << path_s << ':' << line
                << " class=" << class_s
                << " dur_ns=" << dur_ns << "\n";
        return;
    }
}

static VALUE profiler(VALUE, VALUE v) {
    if (tp == Qnil || NIL_P(tp)) {
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
    Debug() << "[PROFILER] Initializing";
    VALUE module = rb_define_module("Profiler");
    tp = rb_tracepoint_new(
        Qnil,
        RUBY_EVENT_CALL | RUBY_EVENT_RETURN,
        tp_cb,
        nullptr
    );
    
    rb_gc_register_address(&tp);    
    rb_define_singleton_method(module, "set", RUBY_METHOD_FUNC(profiler), 1);
}
