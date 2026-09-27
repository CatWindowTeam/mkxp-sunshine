/*
** table-binding.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
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

#include <algorithm>
#include "table.h"
#include "binding-util.h"
#include "serializable-binding.h"

static int num2TableSize(VALUE v){
	int i = NUM2INT(v);
	return std::max(0, i);
}

static void parseArgsTableSizes(int argc, VALUE *argv, int *x, int *y, int *z){
	*y = *z = 1;

	switch (argc){
	case 3:
		*z = num2TableSize(argv[2]);
		/* fall through */
	case 2:
		*y = num2TableSize(argv[1]);
		/* fall through */
	case 1:
		*x = num2TableSize(argv[0]);
		break;
	default:
		rb_error_arity(argc, 1, 3);
	}
}

DEF_TYPE(Table);

static VALUE tableInitialize(int argc, VALUE *argv, VALUE self){
	int x, y, z;
	parseArgsTableSizes(argc, argv, &x, &y, &z);
	Table *t = new Table(x, y, z);
	setPrivateData(self, t);
	return self;
}

static VALUE tableResize(int argc, VALUE *argv, VALUE self){
	Table *t = getPrivateData<Table>(self);
	int x, y, z;
	parseArgsTableSizes(argc, argv, &x, &y, &z);
	t->resize(x, y, z);
	return Qnil;
}

static VALUE tableXSize(VALUE self){
	Table *t = getPrivateData<Table>(self);
	return INT2NUM(t->xSize());
}

static VALUE tableYSize(VALUE self){
	Table *t = getPrivateData<Table>(self);
	return INT2NUM(t->ySize());
}

static VALUE tableZSize(VALUE self){
	Table *t = getPrivateData<Table>(self);
	return INT2NUM(t->zSize());
}

static VALUE tableGetAt(int argc, VALUE *argv, VALUE self){
	Table *t = getPrivateData<Table>(self);
	int x, y, z;
	x = y = z = 0;

	x = NUM2INT(argv[0]);
	if (argc > 1)
		y = NUM2INT(argv[1]);
	if (argc > 2)
		z = NUM2INT(argv[2]);
	if (argc > 3)
		rb_raise(rb_eArgError, "wrong number of arguments");

	if (x < 0 || x >= t->xSize()
	||  y < 0 || y >= t->ySize()
	||  z < 0 || z >= t->zSize()){
		return Qnil;
	}

	short result = t->get(x, y, z);
	return INT2FIX(result); /* short always fits in a Fixnum */
}

static VALUE tableSetAt(int argc, VALUE *argv, VALUE self){
	Table *t = getPrivateData<Table>(self);
	int x, y, z, value;
	x = y = z = 0;
	if (argc < 2)
		rb_raise(rb_eArgError, "wrong number of arguments");

	switch (argc){
	default:
	case 2 :
		x = NUM2INT(argv[0]);
		value = NUM2INT(argv[1]);
		break;
	case 3 :
		x = NUM2INT(argv[0]);
		y = NUM2INT(argv[1]);
		value = NUM2INT(argv[2]);
		break;
	case 4 :
		x = NUM2INT(argv[0]);
		y = NUM2INT(argv[1]);
		z = NUM2INT(argv[2]);
		value = NUM2INT(argv[3]);
		break;
	}

	t->set(value, x, y, z);
	return argv[argc - 1];
}

MARSH_LOAD_FUN(Table)
INITCOPY_FUN(Table)

void tableBindingInit(){
	VALUE klass = rb_define_class("Table", rb_cObject);
	rb_define_alloc_func(klass, classAllocate<&TableType>);
	serializableBindingInit<Table>(klass);
	
	rb_define_singleton_method(klass, "_load", RUBY_METHOD_FUNC(TableLoad), -1);
	rb_define_method(klass, "initialize", RUBY_METHOD_FUNC(tableInitialize), -1);
	rb_define_method(klass, "initialize_copy", RUBY_METHOD_FUNC(TableInitializeCopy), -1);
	rb_define_method(klass, "resize", RUBY_METHOD_FUNC(tableResize), -1);
	rb_define_method(klass, "xsize", RUBY_METHOD_FUNC(tableXSize), 0);
	rb_define_method(klass, "ysize", RUBY_METHOD_FUNC(tableYSize), 0);
	rb_define_method(klass, "zsize", RUBY_METHOD_FUNC(tableZSize), 0);
	rb_define_method(klass, "[]", RUBY_METHOD_FUNC(tableGetAt), -1);
	rb_define_method(klass, "[]=", RUBY_METHOD_FUNC(tableSetAt), -1);
}
