#ifndef GFX_WINDOW_H
# define GFX_WINDOW_H

# include "../math/aliases.h"
# include "shader.h"
# include "texture.h"
# include "buffer.h"
# include "pipeline.h"
# include "color.h"
# include <SDL2/SDL.h>

typedef struct {
	v2i size, resolution, pos;
	color background_color;
	const char *title;
	bool centered;
	unsigned int flags;
}	window_desc;

typedef struct {
	SDL_Window *window;
	SDL_GLContext glcontext;

	color bg_col;

	v2i size, resolution;
	bool fullscreen;
	bool window_fbo;

	buffer_t framebuf;
	buffer_t renderbuf;
	texture_t fbotex;
	pipeline_t pipeline;
	
	shader_t fboshader;
	unsigned int texloc;
}	window_t;

// call SDL_GL_MakeCurrent
// desactivate vsync
// init glew
void	window_create(window_t *window, window_desc window_param);
void	window_destroy(window_t *window);
void	window_update(window_t *window, unsigned int clearBuffers);
void	window_commit(window_t *window);
void	window_fullscreen(window_t *window);

#endif