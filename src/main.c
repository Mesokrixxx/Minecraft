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

typedef struct {
	SDL_Window *window;
	SDL_GLContext glcontext;
	v2i	size;
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

	shader_t shader;
	shader_create(&shader, "res/shaders/sprite.vert", "res/shaders/sprite.frag");
	ASSERT(shader_valid(shader));

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

		SDL_GL_SwapWindow(game.window);
	}

	shader_destroy(&shader);

	SDL_GL_DeleteContext(game.glcontext);
	SDL_DestroyWindow(game.window);
	SDL_Quit();
}
