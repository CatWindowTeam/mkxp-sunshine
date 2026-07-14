#include <ruby.h>
#include <time.h>
#include <SDL3/SDL_stdinc.h>

static VALUE get_month(VALUE self) {
    time_t now = time(NULL);
    struct tm *ltm = localtime(&now);
    return INT2NUM(1 + ltm->tm_mon);
}

static VALUE get_day(VALUE self) {
    time_t now = time(NULL);
    struct tm *ltm = localtime(&now);
    return INT2NUM(ltm->tm_mday);
}

static VALUE ctime_at(int argc, VALUE *argv, VALUE self){
    VALUE v_sec, v_sub = Qnil;
    double dsec;
    long long sec;
    long nsec = 0;

    rb_scan_args(argc, argv, "11", &v_sec, &v_sub);

    if (TYPE(v_sec) == T_FLOAT || rb_obj_is_kind_of(v_sec, rb_cFloat)) {
        dsec = NUM2DBL(v_sec);
        sec = (long long) SDL_floor(dsec);
        double frac = dsec - (double)sec;
        nsec = (long) llround(frac * 1e9);
        if (nsec >= 1000000000L) { sec += 1; nsec -= 1000000000L; }
        if (nsec < 0) {
            sec -= 1;
            nsec += 1000000000L;
        }
    } else {
        sec = NUM2LL(v_sec);
    }

    if (!NIL_P(v_sub)) {
        long usec = NUM2LONG(v_sub);
        long long add_sec = usec / 1000000L;
        long rem_usec = usec % 1000000L;
        if (rem_usec < 0) { rem_usec += 1000000L; add_sec -= 1; }
        sec += add_sec;
        nsec = rem_usec * 1000L + nsec;
        if (nsec >= 1000000000L) { sec += nsec / 1000000000L; nsec = nsec % 1000000000L; }
    }

    return rb_time_new((time_t)sec, (long)nsec);
}

void TimeBindingInit() {
    printf("[TimeBindingInit] Initializing Time binding\n");
    VALUE m = rb_define_module("CTime");
    rb_define_module_function(m, "month", get_month, 0);
    rb_define_module_function(m, "day", get_day, 0);
    rb_define_singleton_method(m, "at", ctime_at, -1);
}
