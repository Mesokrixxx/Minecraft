#include "gfx.h"

#include <stdio.h>

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
