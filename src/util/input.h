#ifndef UTIL_INPUT_H
# define UTIL_INPUT_H

# include "sparseset.h"
# include "dynlist.h"
# include "../math/aliases.h"
# include "../gfx/window.h"

# include <stdbool.h>
# include <SDL2/SDL_scancode.h>
# include <SDL2/SDL_events.h>

typedef enum {
	_INPUT_MOUSE_OFFSET = SDL_NUM_SCANCODES,
	INPUT_MOUSE_LEFT,
	INPUT_MOUSE_MIDDLE,
	INPUT_MOUSE_RIGHT,
	INPUT_MOUSE_SIDE2,
	INPUT_MOUSE_SIDE1,
	INPUT_HANDLED_COUNT,
}	input_mouse_e;

typedef enum {
	INPUT_INVALID = 0 << 0,
	INPUT_PRESSED = 1 << 0,
	INPUT_RELEASED = 1 << 1,
	INPUT_DOWN = 1 << 2,
	INPUT_REPEAT = 1 << 3,
	INPUT_PRESENT = 1 << 7,
}	input_status_e;

typedef struct {
	double last;
	unsigned char state;
}	input_infos_t;

typedef struct {
	window_t *window;

	double now;

	struct {
		bool grab;
		v2i pos, motion;
		v2 scroll;
	}	mouse;

	sparseset_t inputs;
	dynlist_t clear;
}	input_manager_t;

static inline bool	input_manager_valid(int input)
{
	return (input >= 0 && input < INPUT_HANDLED_COUNT);
}

void	input_manager_create(input_manager_t *im, window_t *window);
void	input_manager_destroy(input_manager_t *im);
unsigned char input_manager_get(input_manager_t im, int input);
double input_manager_last(input_manager_t im, int input);
input_infos_t input_manager_infos(input_manager_t im, int input);
void	input_manager_update(input_manager_t *im, double now);
void	input_manager_process(input_manager_t *im, SDL_Event *ev);
void	input_manager_mouse_grab(input_manager_t *im);

#endif