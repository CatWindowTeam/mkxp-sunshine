/*
** gl-meta.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2014 Jonas Kulla <Nyocurio@gmail.com>
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

#include "gl-meta.h"
#include "gl-fun.h"

namespace GLMeta{

#define HAVE_NATIVE_VAO (gl.GenVertexArrays && gl.BindVertexArray && gl.DeleteVertexArrays)

static void vaoBindRes(VAO &vao){
	VBO::bind(vao.vbo);
	IBO::bind(vao.ibo);
	for (size_t i = 0; i < vao.attrCount; ++i){
		const VertexAttribute &va = vao.attr[i];
		gl.EnableVertexAttribArray(va.index);
		gl.VertexAttribPointer(va.index, va.size, va.type, GL_FALSE, vao.vertSize, va.offset);
	}
}

void vaoInit(VAO &vao, bool keepBound){
	if (HAVE_NATIVE_VAO){
		gl.GenVertexArrays(1, &vao.nativeVAO);
		gl.BindVertexArray(vao.nativeVAO);
		vaoBindRes(vao);
		if (!keepBound)
			gl.BindVertexArray(0);
	}else{
		if (keepBound){
			VBO::bind(vao.vbo);
			IBO::bind(vao.ibo);
		}
	}
}

void vaoFini(VAO &vao){
	if (HAVE_NATIVE_VAO)
		gl.DeleteVertexArrays(1, &vao.nativeVAO);
}

void vaoBind(VAO &vao){
	if (HAVE_NATIVE_VAO)
		gl.BindVertexArray(vao.nativeVAO);
	else
		vaoBindRes(vao);
}

void vaoUnbind(VAO &vao){
	if (HAVE_NATIVE_VAO){
		gl.BindVertexArray(0);
	}else{
		for (size_t i = 0; i < vao.attrCount; ++i)
			gl.DisableVertexAttribArray(vao.attr[i].index);

		VBO::unbind();
		IBO::unbind();
	}
}

}
