#ifndef GFX_GFX_H
# define GFX_GFX_H

# include <GL/glew.h>

# ifndef GLAPIENTRY
#  define GLAPIENTRY
# endif 

void	gfx_glcallback_enable();
void	gfx_wireframe();

void GLAPIENTRY gfx_glcallback(
	GLenum source, 
	GLenum type, 
	GLuint id, 
	GLenum severity, 
	GLsizei length, 
	const GLchar* message, 
	const void* userParam);

#endif