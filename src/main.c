#ifdef _WINDOWS_
# define SDL_MAIN_HANDLED
#endif

#define DEFAULT_FPS 200
#define DEFAULT_TPS 60

#define DEFAULT_WINDOW_SIZEX 1080
#define DEFAULT_WINDOW_SIZEY 720
#define DEFAULT_WINDOW_RESX 320
#define DEFAULT_WINDOW_RESY 180

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
	window_t window;

	m4 view, proj;
	
	struct {
		m4 model;
		buffer_t buffer;
		unsigned int bind_point;
	}	vs_params_ubo;

	unsigned int tiles_atlas, font_atlas;
	sprite_manager_t sprite_manager;
	
	input_manager_t input_manager;

	struct {
		double now, delta;

		unsigned int FPS, aimedFPS, render;
		unsigned int TPS, aimedTPS, tick;
		double dtFPS, dtTPS;
	} time;
	
	bool running;
}	instance_t;

void setFPS(instance_t *game, unsigned int FPS)
{
	game->time.aimedFPS = FPS;
	if (FPS > 0)
		game->time.dtFPS = 1.0 / FPS;
}

void setTPS(instance_t *game, unsigned int TPS)
{
	if (!TPS) {
		fprintf(stderr, "Trying to set TPS to 0\n");
		TPS = 1;
	}

	game->time.aimedTPS = TPS;
	game->time.dtTPS = 1.0 / TPS;
}

void init(instance_t *game)
{
	ASSERT(!SDL_Init(SDL_INIT_VIDEO));

	window_create(&game->window, 
		(window_desc){
			.size = v2i_of(DEFAULT_WINDOW_SIZEX, DEFAULT_WINDOW_SIZEY),
			.resolution = v2i_of(DEFAULT_WINDOW_RESX, DEFAULT_WINDOW_RESY),
			.title = "Minecraft",
			.centered = true,
			.flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE,
			.background_color = color_of(0),
			.sh_vertsource = "res/shaders/screenquad.vert",
			.sh_fragsource = "res/shaders/screenquad.frag",
			.uniform_texname = "screenquad",
		});

	gfx_glcallback_enable();

	buffer_create(&game->vs_params_ubo.buffer, BUFFER_UNIFORM, BUFFER_DYNAMIC_DRAW);
	buffer_data(&game->vs_params_ubo.buffer, sizeof(vs_params_t), NULL);
	buffer_bindpoint_assign(game->vs_params_ubo.buffer, &game->vs_params_ubo.bind_point);
	ASSERT(buffer_valid(game->vs_params_ubo.buffer));

	sprite_manager_create(&game->sprite_manager, game->vs_params_ubo.bind_point);

	sprite_manager_register(&game->sprite_manager, &game->tiles_atlas, 
		(sprite_atlas_desc){
			.path = "res/textures/tiles.png",
			.sprite_size = v2i_of(8),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});
	sprite_manager_register(&game->sprite_manager, &game->font_atlas,
		(sprite_atlas_desc){
			.path = "res/textures/font.png",
			.sprite_size = v2i_of(8),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});

	input_manager_create(&game->input_manager, &game->window);

	game->vs_params_ubo.model = m4_identity();
	game->view = m4_identity();
	game->proj = cam_ortho(0, game->window.resolution.x, 0, game->window.resolution.y, 1, -1);

	setFPS(game, DEFAULT_FPS);
	setTPS(game, DEFAULT_TPS);

	game->running = true;
}

void update(instance_t *game)
{
	window_update(&game->window);
	input_manager_update(&game->input_manager, game->time.now);

	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT)
			game->running = false;
		else if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			game->window.size = 
				v2i_of(ev.window.data1, ev.window.data2);
			// game->proj = cam_ortho(0, game->window.size.x, 0, game->window.size.y, 1, -1);
		}
		else
			input_manager_process(&game->input_manager, &ev);
	}
}

void tick(instance_t *game)
{

}

void render(instance_t *game)
{
	vs_params_t params;
	memcpy(params.model, &game->vs_params_ubo.model, sizeof(game->vs_params_ubo.model));
	memcpy(params.view, &game->view, sizeof(game->view));
	memcpy(params.proj, &game->proj, sizeof(game->proj));
	buffer_subdata(&game->vs_params_ubo.buffer, 0, sizeof(vs_params_t), &params);

	sprite_manager_push(&game->sprite_manager, 
		(sprite_t){
			.tex_atlas = game->tiles_atlas,
			.color = color_of(255),
			.z = 0,
			.pos = v2_of(80),
			.index = v2i_of(0),
			.scale = v2_of(1),
		});
	sprite_manager_push(&game->sprite_manager, 
		(sprite_t){
			.tex_atlas = game->font_atlas,
			.color = color_of(255),
			.z = 0,
			.pos = v2_of(50),
			.index = v2i_of(0),
			.scale = v2_of(1),
		});
	sprite_manager_draw(&game->sprite_manager);
	window_commit(&game->window);
}

void destroy(instance_t *game)
{
	input_manager_destroy(&game->input_manager);
	sprite_manager_destroy(&game->sprite_manager);
	buffer_destroy(&game->vs_params_ubo.buffer);
	window_destroy(&game->window);

	SDL_Quit();
}

void process(instance_t *game)
{
	double last = time_s();
	double tick_time = 0;
	double sec_time = 0;

	while (game->running)
	{
		double start = time_s();
		game->time.now = start;
		game->time.delta = start - last;
		last = start;

		tick_time += game->time.delta;
		while (tick_time >= game->time.dtTPS)
		{
			tick(game);
			game->time.tick++;
			tick_time -= game->time.dtTPS;
		}

		sec_time += game->time.delta;
		if (sec_time >= 1.0)
		{
			game->time.FPS = game->time.render;
			game->time.render = 0;

			game->time.TPS = game->time.tick;
			game->time.tick = 0;
			
			sec_time = 0;
			printf("FPS: %u TPS: %u\n", game->time.FPS, game->time.TPS);
		}

		update(game);
		render(game);
		game->time.render++;

		if (game->time.aimedFPS > 0)
		{
			double end = time_s();
			int delay = (int)((game->time.dtFPS - (end - start)) * 1000.0);
			if (delay > 0)
				SDL_Delay(delay);
		}
	}
}

int main() {
	instance_t game = {0};

	init(&game);
	process(&game);
	destroy(&game);	
}
