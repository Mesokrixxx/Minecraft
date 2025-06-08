#include "input.h"

#include <stdio.h>

void	input_manager_create(input_manager_t *im, window_t *window)
{
	*im = (input_manager_t){
		.window = window,
	};

	sparseset_create(&im->inputs, INPUT_HANDLED_COUNT, sizeof(input_infos_t));
	dynlist_create(&im->clear, 32, sizeof(int));

	for (int i = 0; i < INPUT_HANDLED_COUNT; i++)
		sparseset_add(
			&im->inputs,
			&(input_infos_t){ .state = INPUT_PRESENT },
			i);	
}

void	input_manager_destroy(input_manager_t *im)
{
	sparseset_destroy(&im->inputs);
	dynlist_destroy(&im->clear);

	*im = (input_manager_t){0};
}

input_infos_t input_manager_infos(input_manager_t im, unsigned int input)
{
	input_infos_t *infos = sparseset_get(&im.inputs, input);
	
	return infos ? *infos : (input_infos_t){0};
}

void	input_manager_update(input_manager_t *im, double now)
{
	im->now = now;

	im->mouse.motion = v2i_of(0);
	im->mouse.scroll = v2_of(0);

	dynlist_foreach(&im->clear,
		int code = *(int*)(data);
		input_infos_t *infos = sparseset_get(&im->inputs, code);
		infos->state &= ~(INPUT_PRESSED | INPUT_RELEASED););
	dynlist_clear(&im->clear);
}

void	input_manager_process(input_manager_t *im, SDL_Event *ev)
{
	switch (ev->type) {
		case (SDL_MOUSEMOTION):
			im->mouse.motion = 
				v2i_add(
					im->mouse.motion,
					v2i_of(ev->motion.xrel, -ev->motion.yrel));
			im->mouse.pos = 
				v2i_of(ev->motion.x, im->window->size.y - ev->motion.y - 1);
			break ;
		case (SDL_MOUSEWHEEL):
			im->mouse.scroll = 
				v2_add(im->mouse.scroll, v2_of(ev->wheel.preciseX, ev->wheel.preciseY));
			break ;
		case (SDL_MOUSEBUTTONDOWN):
		case (SDL_MOUSEBUTTONUP):
		case (SDL_KEYDOWN):
		case (SDL_KEYUP):
			int code;
			bool down, repeat = false;

			if (ev->type == SDL_KEYDOWN
				|| ev->type == SDL_KEYUP)
			{
				down = ev->type == SDL_KEYDOWN;
				repeat = ev->key.repeat;

				code = ev->key.keysym.scancode;
			} else if (ev->type == SDL_MOUSEBUTTONDOWN
				|| ev->type == SDL_MOUSEBUTTONUP)
			{
				down = ev->type == SDL_MOUSEBUTTONDOWN;

				code = _INPUT_MOUSE_OFFSET + ev->button.button;
			}

			if (code >= INPUT_HANDLED_COUNT)
			{
				fprintf(stderr, "got a non handled input: %d\n", code);
				break ;
			}

			input_infos_t *infos = sparseset_get(&im->inputs, code);
			double last_process = infos->last;
			unsigned char last_state = infos->state;

			if (last_process == im->now && repeat)
				break ; // Reject repeat on same update as press

			if (!(last_state & INPUT_DOWN) || !repeat)
				infos->last = im->now;

			unsigned char new_state = INPUT_PRESENT;

			if (repeat) {
				new_state |= INPUT_DOWN | INPUT_REPEAT;
			}

			if (last_state & INPUT_PRESSED) {
				new_state |= INPUT_PRESSED;
			}

			if (last_state & INPUT_RELEASED) {
				new_state |= INPUT_RELEASED;
			}

			if (down) {
				if (!(last_state & INPUT_DOWN)) {
					new_state |= INPUT_PRESSED;
					dynlist_append(&im->clear, &code);
				}

				new_state |= INPUT_DOWN;
			} else {
				if (last_state & INPUT_DOWN) {
					new_state |= INPUT_RELEASED;
					dynlist_append(&im->clear, &code);
				}
			}

			if ((last_state & INPUT_PRESSED) || (last_state & INPUT_RELEASED)) {
				infos->last = im->now;
			}

			infos->state = new_state;			
			break ;
	};
}

void	input_manager_mouse_grab(input_manager_t *im)
{
	im->mouse.grab = !im->mouse.grab;

	SDL_SetWindowMouseGrab(
		im->window->window, im->mouse.grab ? SDL_TRUE : SDL_FALSE);
	SDL_ShowCursor(im->mouse.grab ? SDL_FALSE : SDL_TRUE);
	SDL_SetRelativeMouseMode(im->mouse.grab ? SDL_TRUE : SDL_FALSE);
}
