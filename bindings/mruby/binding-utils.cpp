#include "binding-utils.h"
inline MRBBindingData* getMrbData(mrb_state *mrb){
	return static_cast<MRBBindingData*>(mrb->ud);
}
