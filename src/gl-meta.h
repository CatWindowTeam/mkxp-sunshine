/*
** gl-meta.h
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

#ifndef GLMETA_H
#define GLMETA_H

#include "gl-fun.h"
#include "gl-util.h"
#include <stddef.h>

struct VertexAttribute{
	GLuint index;
	GLint size;
	GLenum type;
	const GLvoid *offset;
};

namespace GLMeta{

/* ARB_vertex_array_object */
struct VAO{
	/* Set manually, then call vaoInit() */
	const VertexAttribute *attr;
	size_t attrCount;
	GLsizei vertSize;
	VBO::ID vbo;
	IBO::ID ibo;

	/* Don't touch */
	GLuint nativeVAO;
};

void vaoInit(VAO &vao, bool keepBound = false);
void vaoFini(VAO &vao);
void vaoBind(VAO &vao);
void vaoUnbind(VAO &vao);

}

#endif // GLMETA_H
