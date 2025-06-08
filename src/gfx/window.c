#include "window.h"

#include "../util/assert.h"
#include <GL/glew.h>

typedef struct {
	v2 pos, texcoord;
}	vertices_t;

void	window_create(window_t *window, window_desc window_param)
{
	*window = (window_t){
		.size = window_param.size,
		.resolution = window_param.resolution,
		.bg_col = window_param.background_color,
	};

	ASSERT(SDL_WasInit(SDL_INIT_VIDEO),
		"SDL video need to be initialized");

	v2i wpos = window_param.pos;
	if (window_param.centered)
		wpos = v2i_of(SDL_WINDOWPOS_CENTERED_DISPLAY(1));
	window->window =
		SDL_CreateWindow(
			window_param.title,
			wpos.x, wpos.y,
			window_param.size.x, window_param.size.y,
			window_param.flags);
	ASSERT(window->window, "Failed to create SDL window");

	window->glcontext = 
		SDL_GL_CreateContext(window->window);
	ASSERT(window->glcontext, "Failed to create GL context");

	SDL_GL_MakeCurrent(window->window, window->glcontext);
	SDL_GL_SetSwapInterval(0);

	ASSERT(glewInit() == GLEW_OK, "failed to load glew");

	buffer_create(&window->framebuf, BUFFER_FRAME);

	shader_create(&window->fboshader, window_param.sh_vertsource, window_param.sh_fragsource);
	shader_uniform_loc(window->fboshader, window_param.uniform_texname, &window->texloc);
	ASSERT(shader_valid(window->fboshader), "failed to create window fbo shader");
	
	texture_create(&window->fbotex, TEXTURE_2D);
	texture_sampler_set(&window->fbotex, 
		(texture_sampler_desc){
			.wrap_u = TEXTURE_CLAMP_TO_EDGE,
			.wrap_v = TEXTURE_CLAMP_TO_EDGE,
			.min_filter = TEXTURE_NEAREST,
			.max_filter = TEXTURE_NEAREST,
		});
	texture_data_set(&window->fbotex,
		(texture_data_desc){
			.size = window_param.resolution,
			.internal_format = TEXTURE_RGBA,
			.format = TEXTURE_RGBA,
			.datatype = TEXTURE_UNSIGNED_BYTE,
		});
	ASSERT(texture_valid(window->fbotex), "failed to create window fbo texture");

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, 
		window->fbotex.id,
		0);
	
	buffer_create(&window->renderbuf, BUFFER_RENDER);
	glRenderbufferStorage(
		GL_RENDERBUFFER, 
		GL_DEPTH_COMPONENT24, 
		window_param.resolution.x, window_param.resolution.y);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, window->renderbuf.id);

	ASSERT(buffer_valid(window->renderbuf), "failed to create render buffer");
	ASSERT(buffer_valid(window->framebuf), "failed to create framebuffer");

	glUniform1i(window->texloc, window->fbotex.bind_point);

	unsigned short indices[] = {2, 1, 0, 3, 2, 0};
	vertices_t vertices[] = {
		{ v2_of(-1, 1), v2_of(0, 1) },
		{ v2_of(1, 1), v2_of(1, 1) },
		{ v2_of(1, -1), v2_of(1, 0) },
		{ v2_of(-1, -1), v2_of(0, 0) },
	};

	pipeline_create(&window->pipeline, window->fboshader, 2,
		(pipeline_buffers_desc[]){
			{
				.type = BUFFER_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(vertices),
				.stride = sizeof(vertices_t),
				.data = vertices,
			},
			{
				.type = BUFFER_ELEMENT_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(indices),
				.data = indices,
			}
		});
	pipeline_attributes_assign(&window->pipeline, 2,
		(pipeline_attributes_desc[]){
			{
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(vertices_t, pos),
			},
			{
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(vertices_t, texcoord),
			}
		});
	pipeline_render_setup(&window->pipeline, 
		(pipeline_render_desc){
			.method = PIPELINE_ELEMENTS,
			.type = PIPELINE_TRIANGLES,
			.element_count = 6,
			.element_type = PIPELINE_UNSIGNED_SHORT,
		});
}

void	window_destroy(window_t *window)
{
	pipeline_destroy(&window->pipeline);
	shader_destroy(&window->fboshader);
	buffer_destroy(&window->renderbuf);
	buffer_destroy(&window->framebuf);

	SDL_GL_DeleteContext(window->glcontext);
	SDL_DestroyWindow(window->window);

	*window = (window_t){0};
}

void	window_update(window_t *window)
{
	buffer_bind(window->framebuf);
	glViewport(0, 0, window->resolution.x, window->resolution.y);

	glClearColor(
		window->bg_col.r, window->bg_col.g,
		window->bg_col.b, window->bg_col.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void	window_commit(window_t *window)
{
	buffer_bind((buffer_t){ .type = BUFFER_FRAME, .id = 0 });
	glViewport(0, 0, window->size.x, window->size.y);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	pipeline_bind(window->pipeline);
	pipeline_render(window->pipeline);

	SDL_GL_SwapWindow(window->window);
}
