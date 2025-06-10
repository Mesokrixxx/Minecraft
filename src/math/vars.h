#ifndef MATH_VARS_H
# define MATH_VARS_H

# include <cglm/types.h>

# define PI GLM_PI
# define PI_2 GLM_PI_2
# define TAU (2 * PI)

# define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); (_a > _b ? _a : _b); })
# define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); (_a > _b ? _b : _a); })
# define clamp(a, b, c) max(b, min(a, c))
# define rad(d) (((d) * PI) / 180)
# define deg(r) (((r) * 180) / PI)

#endif