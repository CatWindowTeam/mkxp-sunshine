#include "exception.h"

void crash(Exception::Type t, bool do_crash, const char *fmt, ...);
void ErrorMsg(const char* message);
void WarnMsg(const char* message);
