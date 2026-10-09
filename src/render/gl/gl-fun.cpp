/*
** gl-fun.cpp
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

#include "gl-fun.h"
#include "glrender.h"
#include "meow.h"
#include <SDL3/SDL_video.h>
#include <string>

GLFunctions gl;

typedef const GLubyte* (APIENTRYP _PFNGLGETSTRINGIPROC) (GLenum, GLuint);
#define GL_FUN(name, type) \
	gl.name = (type) SDL_GL_GetProcAddress("gl" #name EXT_SUFFIX);

//https://github.com/elizagamedev/mkxp-oneshot/blob/master/src/main.cpp
static inline const char* glGetStringInt(GLenum name){
	return (const char*) gl.GetString(name);
}

void initGLFunctions(){
#define EXT_SUFFIX ""
	GL_20_FUN;

	//https://github.com/elizagamedev/mkxp-oneshot/blob/master/src/main.cpp
	Debug() << "GL Vendor: " << glGetStringInt(GL_VENDOR);
	Debug() << "GL Renderer: " << glGetStringInt(GL_RENDERER);
	Debug() << "GL Version: " << glGetStringInt(GL_VERSION);
		
	/* Determine GL version */
	const char *ver = (const char*) gl.GetString(GL_VERSION);
	const char glesPrefix[] = "OpenGL ES ";
	const size_t glesPrefixN = sizeof(glesPrefix)-1;
	bool gles = false;
	if (!SDL_strncmp(ver, glesPrefix, glesPrefixN)){
		gles = true;
		gl.glsles = true;
		ver += glesPrefixN;
	}

	/* Assume single digit */
	int glMajor = *ver - '0';
	if (glMajor < 2){
		#ifdef GLES2_HEADER
			ErrorMsg("At least OpenGL ES 2.0 is required");
		#else
			ErrorMsg("At least OpenGL 2.0 is required");
		#endif
	}

	if (gles){
		GL_ES_FUN;
	}

	/* FBO entrypoints */
	if (glMajor >= 3 || SDL_GL_ExtensionSupported("GL_ARB_framebuffer_object")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX ""
		GL_FBO_FUN;
		GL_FBO_BLIT_FUN;
	}else if(gles && glMajor == 2){
		GL_FBO_FUN;
	}else if(SDL_GL_ExtensionSupported("GL_EXT_framebuffer_object")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX "EXT"
		GL_FBO_FUN;
		if(SDL_GL_ExtensionSupported("GL_EXT_framebuffer_blit")){
			GL_FBO_BLIT_FUN;
		}
	}else{
		ErrorMsg("No FBO support available");
	}

	/* VAO entrypoints */
	if(SDL_GL_ExtensionSupported("GL_ARB_vertex_array_object") || glMajor >= 3){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX ""
		GL_VAO_FUN;
		gl.vao = true;
	}else if(SDL_GL_ExtensionSupported("GL_APPLE_vertex_array_object")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX "APPLE"
		GL_VAO_FUN;
		gl.vao = true;
	}else if(SDL_GL_ExtensionSupported("GL_OES_vertex_array_object")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX "OES"
		GL_VAO_FUN;
		gl.vao = true;
	}

	/* Debug callback entrypoints */
	if(SDL_GL_ExtensionSupported("GL_KHR_debug")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX ""
		GL_DEBUG_KHR_FUN;
	}else if(SDL_GL_ExtensionSupported("GL_ARB_debug_output")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX "ARB"
		GL_DEBUG_KHR_FUN;
	}

	if(SDL_GL_ExtensionSupported("GL_GREMEDY_string_marker")){
		#undef EXT_SUFFIX
		#define EXT_SUFFIX "GREMEDY"
		GL_GREMEMDY_FUN;
	}

	/* Misc caps */
	if (!gles || glMajor >= 3 || SDL_GL_ExtensionSupported("GL_EXT_unpack_subimage")){
		gl.unpack_subimage = true;	
	}

	if (!gles || glMajor >= 3 || SDL_GL_ExtensionSupported("GL_OES_texture_npot")){
		gl.npot_repeat = true;
	}
}

