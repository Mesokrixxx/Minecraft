#ifndef GFX_COLOR_H
# define GFX_COLOR_H

# include "../util/macros.h"
# include "../math/aliases.h"

typedef v4	color;

# define _COLOF4(r, g, b, a) ((color){{ r / 255.0, g / 255.0, b / 255.0, a / 255.0 }})
# define _COLOF3(r, g, b) ((color){{ r / 255.0, g / 255.0, b / 255.0, 1 }})
# define _COLOF1(rgb) ({ float _rgb = (rgb) / 255.0; ((color){{ _rgb, _rgb, _rgb, 1 }}); })
# define _COLOF0() ((color){{ 0, 0, 0, 1 }})
# define color_of(...) VMACRO(_COLOF, __VA_ARGS__)

#endif