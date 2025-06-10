#include "gfx/color.h"
#include <SDL2/SDL_scancode.h>
#ifdef _WINDOWS_
# define SDL_MAIN_HANDLED
#endif

#define DEFAULT_FPS 240
#define DEFAULT_TPS 60

#define DEFAULT_WINDOW_SIZEX 1080
#define DEFAULT_WINDOW_SIZEY 720

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdbool.h>

#include "gfx/window.h"
#include "gfx/gfx.h"
#include "util/input.h"
#include "util/assert.h"
#include "util/time.h"
#include "util/camera.h"
#include "math/vars.h"
#include "sprite.h"
#include "font.h"
#include "ui.h"

typedef struct {
	mat4 model;
	mat4 view;
	mat4 proj;
}	vs_params_t;

typedef struct {
	window_t window;
	camera_t camera;

	m4 view, proj;
	
	struct {
		m4 model;
		buffer_t buffer;
		unsigned int bind_point;
	}	vs_params_ubo;

	ui_t ui;

	unsigned int tiles_atlas;
	sprite_manager_t sprite_manager;
	
	input_manager_t input_manager;

	struct {
		double now, delta_frame, delta_tick;

		unsigned int FPS, aimedFPS, render;
		unsigned int TPS, aimedTPS, tick;
		double aimedDTFrame, aimedDTTick;
	} time;
	
	bool running;
}	instance_t;

void setFPS(instance_t *game, unsigned int FPS)
{
	game->time.aimedFPS = FPS;
	if (FPS > 0)
		game->time.aimedDTFrame = 1.0 / FPS;
}

void setTPS(instance_t *game, unsigned int TPS)
{
	if (!TPS) {
		fprintf(stderr, "Trying to set TPS to 0\n");
		TPS = 1;
	}

	game->time.aimedTPS = TPS;
	game->time.aimedDTTick = 1.0 / TPS;
}

void init(instance_t *game)
{
	ASSERT(!SDL_Init(SDL_INIT_VIDEO));

	window_create(&game->window, 
		(window_desc){
			.size = v2i_of(DEFAULT_WINDOW_SIZEX, DEFAULT_WINDOW_SIZEY),
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
	font_init(&game->sprite_manager);
	input_manager_create(&game->input_manager, &game->window);
	ui_init(&game->ui, &game->window,
		(vs_params_ubo_desc){
			.buffer = &game->vs_params_ubo.buffer,
			.model = &game->vs_params_ubo.model,
		});

	m4 model = m4_identity();
	game->vs_params_ubo.model = model;
	camera_init(&game->camera, 
		(camera_desc){
			.window = &game->window,
			.type = CAMERA_PERSPECTIVE,
			.znear = 0.1,
			.zfar = 1000.0,
			.perspective = (camera_perspective_desc){
				.fov = rad(75.0),
			}
		});

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
			ui_event_windowresize(&game->ui);
		}
		else
			input_manager_process(&game->input_manager, &ev);
	}

	if (input_manager_get(game->input_manager, SDL_SCANCODE_ESCAPE) & INPUT_PRESSED)
		input_manager_mouse_grab(&game->input_manager);
	if (input_manager_get(game->input_manager, SDL_SCANCODE_F11) & INPUT_PRESSED)
		window_fullscreen(&game->window);

	camera_update(&game->camera);

	if (game->input_manager.mouse.grab) {
		game->camera.persp.pitch += game->input_manager.mouse.motion.y / 800.0;
		game->camera.persp.yaw -= game->input_manager.mouse.motion.x / 800.0;
	}
}

void tick(instance_t *game)
{
	if (game->input_manager.mouse.grab) {
		float camspeed = 100.0 * game->time.delta_tick;

		v3 up = v3_of(0, 1, 0);
		v3 forward = v3_of(sinf(game->camera.persp.yaw), 0, cosf(game->camera.persp.yaw));
		v3 right = v3_cross(up, forward);
		if (input_manager_get(game->input_manager, SDL_SCANCODE_W) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(forward, camspeed));
		else if (input_manager_get(game->input_manager, SDL_SCANCODE_S) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(forward, -camspeed));

		if (input_manager_get(game->input_manager, SDL_SCANCODE_A) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(right, camspeed));
		else if (input_manager_get(game->input_manager, SDL_SCANCODE_D) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(right, -camspeed));

		if (input_manager_get(game->input_manager, SDL_SCANCODE_SPACE) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(up, camspeed));
		else if (input_manager_get(game->input_manager, SDL_SCANCODE_LSHIFT) & INPUT_DOWN)
			game->camera.persp.pos = v3_add(game->camera.persp.pos, v3_scale(up, -camspeed));
	}
}

void render(instance_t *game)
{
	vs_params_t params;
	memcpy(params.model, &game->vs_params_ubo.model, sizeof(game->vs_params_ubo.model));
	memcpy(params.view, &game->camera.view, sizeof(game->view));
	memcpy(params.proj, &game->camera.projection, sizeof(game->proj));
	buffer_subdata(&game->vs_params_ubo.buffer, 0, sizeof(vs_params_t), &params);

	sprite_manager_draw(&game->sprite_manager);
	ui_pass(game->ui);

	char debug[512];
	sprintf(debug, "FPS:%u\nTPS:%u",
		game->time.FPS, game->time.TPS);
	font_str(debug, 
		(font_desc){
			.color = color_of(255),
			.z = 0,
			.pos = v2_of(0, game->window.size.y - 3 * 8),
			.scale = v2_of(3),
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
	double frame_last = time_s();
	double tick_last = frame_last;
	double tick_time = 0;
	double sec_time = 0;

	while (game->running)
	{
		double frame_start = time_s();
		game->time.now = frame_start;
		game->time.delta_frame = frame_start - frame_last;
		frame_last = frame_start;

		tick_time += game->time.delta_frame;
		while (tick_time >= game->time.aimedDTTick)
		{
			tick(game);

			double tick_start = time_s();
			game->time.delta_tick = tick_start - tick_last;
			tick_last = tick_start;
			game->time.tick++;
			tick_time -= game->time.aimedDTTick;
		}

		sec_time += game->time.delta_frame;
		if (sec_time >= 1.0)
		{
			game->time.FPS = game->time.render;
			game->time.render = 0;

			game->time.TPS = game->time.tick;
			game->time.tick = 0;
			
			sec_time = 0;
		}

		update(game);
		render(game);
		game->time.render++;

		if (game->time.aimedFPS > 0)
		{
			double frame_end = time_s();
			int delay = (int)((game->time.aimedDTFrame - (frame_end - frame_start)) * 1000.0);
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
