#ifdef _WINDOWS_
# define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdbool.h>

#include "math/math.h"
#include "gfx/gfx.h"
#include "util/util.h"

#include "sprite.h"

typedef struct {
	mat4 model;
	mat4 view;
	mat4 proj;
}	vs_params_t;

typedef struct {
	SDL_Window *window;
	SDL_GLContext glcontext;
	v2i	size;
	struct {
		buffer_t buffer;
		unsigned int bind_point;
	}	vs_params_ubo;
	sprite_manager_t sprite_manager;
	bool running;
}	instance_t;

int main() {
	instance_t game = {0};

	ASSERT(!SDL_Init(SDL_INIT_VIDEO));

	game.size = v2i_of(1080, 720);
	game.window = 
		SDL_CreateWindow(
			"Minecraft",
			SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
			game.size.x, game.size.y,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	ASSERT(game.window);

	game.glcontext = SDL_GL_CreateContext(game.window);
	ASSERT(game.glcontext);

	SDL_GL_MakeCurrent(game.window, game.glcontext);

	ASSERT(glewInit() == GLEW_OK);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gfx_glcallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	buffer_create(&game.vs_params_ubo.buffer, BUFFER_UNIFORM, BUFFER_DYNAMIC_DRAW);
	buffer_data(&game.vs_params_ubo.buffer, sizeof(vs_params_t), NULL);
	buffer_bindpoint_assign(game.vs_params_ubo.buffer, &game.vs_params_ubo.bind_point);
	sprite_manager_create(&game.sprite_manager, game.vs_params_ubo.bind_point);

	m4 identity = m4_identity();
	m4 proj = cam_ortho(0, game.size.x, game.size.y, 0, 1, -1);

	game.running = true;
	while (game.running)
	{
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case (SDL_QUIT):
					game.running = false;
					break ;
			}
		}

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		vs_params_t params;
		memcpy(params.model, &identity, sizeof(identity));
		memcpy(params.view, &identity, sizeof(identity));
		memcpy(params.proj, &proj, sizeof(proj));
		buffer_subdata(&game.vs_params_ubo.buffer, 0, sizeof(vs_params_t), &params);

		for (int i = 0; i < 100; i++)
			sprite_manager_push(&game.sprite_manager, 
				(sprite_t){
					.color = color_of(255),
					.z = 0,
					.pos = v2_of(50 + i * 2, 50 + i),
				});
		sprite_manager_draw(&game.sprite_manager);

		SDL_GL_SwapWindow(game.window);
	}

	sprite_manager_destroy(&game.sprite_manager);
	buffer_destroy(&game.vs_params_ubo.buffer);

	SDL_GL_DeleteContext(game.glcontext);
	SDL_DestroyWindow(game.window);
	SDL_Quit();
}
