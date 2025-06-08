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
	input_manager_t input_manager;
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
	ASSERT(buffer_valid(game.vs_params_ubo.buffer));

	sprite_manager_create(&game.sprite_manager, game.vs_params_ubo.bind_point);

	unsigned int tiles_atlas, font_atlas;
	sprite_manager_register(&game.sprite_manager, &tiles_atlas, 
		(sprite_atlas_desc){
			.path = "res/textures/tiles.png",
			.sprite_size = v2i_of(8),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});
	sprite_manager_register(&game.sprite_manager, &font_atlas,
		(sprite_atlas_desc){
			.path = "res/textures/font.png",
			.sprite_size = v2i_of(8),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});

	input_manager_create(&game.input_manager, game.window);

	m4 identity = m4_identity();
	m4 proj = cam_ortho(0, game.size.x, 0, game.size.y, 1, -1);

	game.running = true;
	while (game.running)
	{
		static double last, accumulated_time;
		double now = time_s();
		double delta = now - last;
		last = now;

		accumulated_time += delta;
		if (accumulated_time >= 1)
		{
			printf("FPS: %u\n", 1.0 / delta);
			accumulated_time = 0;
		}

		input_manager_update(&game.input_manager, now, game.size);

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				game.running = false;
			else
				input_manager_process(&game.input_manager, &ev);
		}

		unsigned char state = input_manager_get(game.input_manager, SDL_SCANCODE_A);
		double laststatechange = input_manager_last(game.input_manager, SDL_SCANCODE_A);
		if (state & INPUT_PRESSED)
			printf("PRESSED A (lastupdate: %f)\n", laststatechange);
		if (state & INPUT_RELEASED)
			printf("RELEASED A (lastupdate: %f)\n", laststatechange);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		vs_params_t params;
		memcpy(params.model, &identity, sizeof(identity));
		memcpy(params.view, &identity, sizeof(identity));
		memcpy(params.proj, &proj, sizeof(proj));
		buffer_subdata(&game.vs_params_ubo.buffer, 0, sizeof(vs_params_t), &params);

		sprite_manager_push(&game.sprite_manager, 
			(sprite_t){
				.tex_atlas = tiles_atlas,
				.color = color_of(255),
				.z = 0,
				.pos = v2_of(50, 50),
				.index = v2i_of(0),
				.scale = v2_of(4),
			});
		sprite_manager_push(&game.sprite_manager, 
			(sprite_t){
				.tex_atlas = font_atlas,
				.color = color_of(255),
				.z = 0,
				.pos = v2_of(200, 200),
				.index = v2i_of(0),
				.scale = v2_of(4),
			});
		sprite_manager_draw(&game.sprite_manager);

		SDL_GL_SwapWindow(game.window);
	}

	input_manager_destroy(&game.input_manager);
	sprite_manager_destroy(&game.sprite_manager);
	buffer_destroy(&game.vs_params_ubo.buffer);

	SDL_GL_DeleteContext(game.glcontext);
	SDL_DestroyWindow(game.window);
	SDL_Quit();
}
