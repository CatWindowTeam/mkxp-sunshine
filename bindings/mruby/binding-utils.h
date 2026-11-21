#pragma once
#include <mruby.h>
#include <stdbool.h>
#include "data.h"

#define DEF_TYPE_CUSTOMNAME_AND_FREE(Klass, Name, Free) \
  static const mrb_data_type Klass##Type = { Name, Free }

#define DEF_TYPE(Klass) DEF_TYPE_CUSTOMNAME(Klass, #Klass)

inline MRBBindingData* getMrbData(mrb_state *mrb);


inline void setPrivateData(mrb_value self, void *p){
  DATA_PTR(self) = p;
}

inline mrb_value wrapObject(mrb_state *mrb, void *p, const char *wrapName, const mrb_data_type &type, mrb_value underKlass){
    mrb_value klass = mrb_const_get(mrb, underKlass, mrb_intern_cstr(mrb, wrapName));
    return mrb_obj_value(mrb_data_object_alloc(mrb, mrb_class_ptr(klass), p, &type));
}

inline mrb_value wrapObject(mrb_state *mrb, void *p, const char *wrapName, const mrb_data_type &type){
    return wrapObject(mrb, p, wrapName, type, mrb_obj_value(mrb->object_class));
}

inline mrb_value wrapProperty(mrb_state *mrb, mrb_value self, void *prop, const char *iv, const char *wrapName, const mrb_data_type &type, mrb_value underKlass){
    mrb_value propObj = wrapObject(mrb, prop, wrapName, type, underKlass);
    mrb_iv_set(mrb, self, mrb_intern_cstr(mrb, iv), propObj);
    return propObj;
}

inline bool klass_initialize_copy(mrb_state *mrb, mrb_value self){
    mrb_value orig;
    mrb_get_args(mrb, "o", &orig);
	return true;
}

template <class C>
inline C *getPrivateData(mrb_state *mrb, mrb_value self, const mrb_data_type &type){
    C *c = static_cast<C *>(mrb_data_get_ptr(mrb, self, &type));
    if (!c) {
        mrb_raise(mrb, E_RUNTIME_ERROR, "No instance data for variable (missing call to super?)");
    }
    return c;
}
