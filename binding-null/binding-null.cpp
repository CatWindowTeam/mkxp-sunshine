/*
** binding-null.cpp
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

#include "binding.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"
#include "define.h"
#include "meow.h"

//so this binding need only for debug and engine porting:)

static void nullBindingExecute(){
	ErrorMsg("Hello from PSVita!");
	shState->rtData().rqTermAck.set();
}

static void nullBindingTerminate(){}
static void nullBindingReset(){}
static void nullBindingGc(){}

ScriptBinding scriptBindingImpl ={
    nullBindingExecute,
    nullBindingTerminate,
    nullBindingReset,
    nullBindingGc
};

ScriptBinding *scriptBinding = &scriptBindingImpl;
