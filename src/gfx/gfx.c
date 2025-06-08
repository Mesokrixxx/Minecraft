#include "gfx.h"

#include <stdio.h>

void gfx_glcallback_enable()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gfx_glcallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

void GLAPIENTRY gfx_glcallback(
	GLenum source, 
	GLenum type,
	GLuint id,
	GLenum severity, 
	GLsizei length, 
	const GLchar* message, 
	const void* userParam)
{    
    fprintf(stderr, ">>> %s\n", message);
}
