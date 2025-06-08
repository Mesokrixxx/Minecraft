#ifndef GFX_GFX_H
# define GFX_GFX_H

# include "shader.h"
# include "buffer.h"
# include "pipeline.h"
# include "color.h"
# include "texture.h"
# include "window.h"

# ifndef GLAPIENTRY
#  define GLAPIENTRY
# endif 

void	gfx_glcallback_enable();

void GLAPIENTRY gfx_glcallback(
	GLenum source, 
	GLenum type, 
	GLuint id, 
	GLenum severity, 
	GLsizei length, 
	const GLchar* message, 
	const void* userParam);

#endif