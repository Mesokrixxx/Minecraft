#ifndef UTIL_TIME_H
# define UTIL_TIME_H

# define TIME_NS_TO_S(ns) ((ns) / 1000000000.0)
# define TIME_S_TO_NS(s) ((s) * 1000000000.0)

unsigned long long time_ns();

static inline double time_s() {
	return (TIME_NS_TO_S(time_ns()));
}

#endif