#include <ruby.h>
#include <time.h>

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

void TimeBindingInit() {
    printf("[TimeBindingInit] Initializing Time binding\n");
    VALUE m = rb_define_module("CTime");
    rb_define_module_function(m, "month", get_month, 0);
    rb_define_module_function(m, "day", get_day, 0);
}
