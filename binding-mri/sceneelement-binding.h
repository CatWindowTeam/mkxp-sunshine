/*
** sceneelement-binding.h
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is SDL_free software: you can redistribute it and/or modify
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

#pragma once
#include "scene.h"
#include "binding-util.h"

template<class C>
static VALUE sceneElementGetZ(VALUE self){
	SceneElement *se = getPrivateData<C>(self);
	int value = 0;
	GUARD_EXC( value = se->getZ(); );
	return rb_fix_new(value);
}

template<class C>
static VALUE sceneElementSetZ(int argc, VALUE *argv, VALUE self){
	SceneElement *se = getPrivateData<C>(self);

	int z;
	rb_get_args(argc, argv, "i", &z RB_ARG_END);
	GUARD_EXC( se->setZ(z); );
	return rb_fix_new(z);
}

template<class C>
static VALUE sceneElementGetVisible(VALUE self){
	SceneElement *se = getPrivateData<C>(self);
	bool value = false;
	GUARD_EXC( value = se->getVisible(); );
	return rb_bool_new(value);
}

template<class C>
static VALUE sceneElementSetVisible(int argc, VALUE *argv, VALUE self){
	SceneElement *se = getPrivateData<C>(self);
	bool visible;
	rb_get_args(argc, argv, "b", &visible RB_ARG_END);
	GUARD_EXC( se->setVisible(visible); );
    return rb_bool_new(visible);
}

template<class C>
void sceneElementBindingInit(VALUE klass){
	rb_define_method(klass, "z", RUBY_METHOD_FUNC(sceneElementGetZ<C>), 0);
	rb_define_method(klass, "z=", RUBY_METHOD_FUNC(sceneElementSetZ<C>), -1);
	rb_define_method(klass, "visible", RUBY_METHOD_FUNC(sceneElementGetVisible<C>), 0);
	rb_define_method(klass, "visible=", RUBY_METHOD_FUNC(sceneElementSetVisible<C>), -1);
}
