#include "binding-mruby/data.h"
#include <mruby.h>
#include <mruby/error.h>
#include <stdexcept>

void exceptions_binding(mrb_state *mrb){
  auto standardError = mrb_class_get(mrb, "StandardError");
  if (!standardError) {
		throw std::runtime_error("failed to acquire StandardError");
  }

  auto shutdownError = mrb_define_class(mrb, "ShutdownError", standardError);

	MRBBindingData* data = static_cast<MRBBindingData*>(mrb->ud);
	if (data != nullptr) {
    data->shudownExc = shutdownError;
  } else {
		throw std::runtime_error("failed to acquire MRB binding data");
  }
}
