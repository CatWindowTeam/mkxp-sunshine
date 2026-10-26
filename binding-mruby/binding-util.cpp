/*
** binding-util.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 - 2021 Amaryllis Kulla <ancurio@mapleshrine.eu>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "binding-util.h"

void raiseDisposedAccess(mrb_state *mrb, mrb_value self){
	const char *klassName = DATA_TYPE(self)->struct_name;
	char buf[32];

	SDL_strlcpy(buf, klassName, sizeof(buf));
	buf[0] = tolower(buf[0]);

	mrb_raisef(mrb, E_RUNTIME_ERROR, "disposed %S", mrb_str_new_cstr(mrb, buf));
}

MRB_METHOD_PUB(inspectObject){
	static char buffer[64];
	SDL_snprintf(buffer, sizeof(buffer), "#<%s:%p>", DATA_TYPE(self)->struct_name, DATA_PTR(self));
	return mrb_str_new_cstr(mrb, buffer);
}
