#include "time.h"

#include <SDL2/SDL_timer.h>

static unsigned long long time_start, time_freq;

unsigned long long time_ns()
{
	unsigned long long now = SDL_GetPerformanceCounter();

	if (!time_start)
		time_start = now;

	if (!time_freq)
		time_freq = SDL_GetPerformanceFrequency();

	unsigned long long diff = now - time_start;
	return (TIME_S_TO_NS(diff / (double) time_freq));
}
