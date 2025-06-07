#ifndef UTIL_ASSERT_H
# define UTIL_ASSERT_H

# include "macros.h"

# include <stdio.h>
# include <stdlib.h>

# define _ASSERTX(cond, expr, fmt, ...) \
	do { \
		if (!(cond)) \
		{ \
			fprintf(stderr, "Assertion failed: [%s] (%s::%s::%d)\n>>> " fmt "\n", \
			expr, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
			exit(1); \
		} \
	} while (0); \

# define _ASSERT8(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT7(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT6(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT5(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT4(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT3(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT2(cond, ...) _ASSERTX(cond, #cond, __VA_ARGS__)
# define _ASSERT1(cond) _ASSERTX(cond, #cond, "Seems like you still got work to do")
# define _ASSERT0() _ASSERTX(0, "false", "Forced assertion")
# define ASSERT(...) VMACRO(_ASSERT, __VA_ARGS__)

#endif