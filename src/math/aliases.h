#ifndef MATH_ALIASES_H
# define MATH_ALIASES_H

# include <cglm/struct/vec2.h>
# include <cglm/struct/vec3.h>
# include <cglm/struct/vec4.h>
# include <cglm/struct/mat4.h>
# include <cglm/struct/cam.h>

# include "../util/macros.h"

# define m4_identity glms_mat4_identity

# define cam_ortho glms_ortho

typedef vec2s	v2;
typedef ivec2s	v2i;
typedef vec3s	v3;
typedef ivec3s	v3i;
typedef vec4s	v4;
typedef ivec4s	v4i;
typedef mat4s	m4;

# define _V2OF2(x, y) ((v2){{ x, y }})
# define _V2OF1(xy)	({ float _xy = (xy); ((v2){{ _xy, _xy }}); })
# define _V2OF0() ((v2){{ 0, 0 }})
# define v2_of(...) VMACRO(_V2OF, __VA_ARGS__)

# define _V2IOF2(x, y) ((v2i){{ x, y }})
# define _V2IOF1(xy) ({ int _xy = (xy); ((v2i){{ _xy, _xy }}); })
# define _V2IOF0() ((v2i){{ 0, 0 }})
# define v2i_of(...) VMACRO(_V2IOF, __VA_ARGS__)

# define _V3OF3(x, y, z) ((v3){{ x, y, z }})
# define _V3OF2(v2, z) ({ __typeof__(v2) _v2 = (v2); ((v3){{ _v2.x, _v2.y, z }}); })
# define _V3OF1(xyz) ({ float _xyz = (xyz); ((v3){{ _xyz, _xyz, _xyz }}); })
# define _V3OF0() ((v3){{ 0, 0, 0 }})
# define v3_of(...) VMACRO(_V3OF, __VA_ARGS__)

# define _V3IOF3(x, y, z) ((v3i){{ x, y, z }})
# define _V3IOF2(v2, z) ({ __typeof__(v2) _v2 = (v2); ((v3i){{ _v2.x, _v2.y, z }}); })
# define _V3IOF1(xyz) ({ int _xyz = (xyz); ((v3i){{ _xyz, _xyz, _xyz }}); })
# define _V3IOF0() ((v3i){{ 0, 0, 0 }})
# define v3i_of(...) VMACRO(_V3IOF, __VA_ARGS__)

# define _V4OF4(x, y, z, w) ((v4){{ x, y, z, w }})
# define _V4OF3(v2, z, w) ({ __typeof__(v2) _v2 = (v2); ((v4){{ _v2.x, _v2.y, z, w }}); })
# define _V4OF2(v3, w) ({ __typeof__(v3) _v3 = (v3); ((v4){{ _v3.x, _v3.y, _v3.z, w }}); })
# define _V4OF1(xyzw) ({ float _xyzw = (xyzw); ((v4){{ _xyzw, _xyzw, _xyzw, _xyzw }}); })
# define _V4OF0() ((v4){{ 0, 0, 0, 0 }})
# define v4_of(...) VMACRO(_V4OF, __VA_ARGS__)

# define _V4IOF4(x, y, z, w) ((v4i){{ x, y, z, w }})
# define _V4IOF3(v2, z, w) ({ __typeof__(v2) _v2 = (v2); ((v4i){{ _v2.x, _v2.y, z, w }}); })
# define _V4IOF2(v3, w) ({ __typeof__(v3) _v3 = (v3); ((v4i){{ _v3.x, _v3.y, _v3.z, w }}); })
# define _V4IOF1(xyzw) ({ float _xyzw = (xyzw); ((v4i){{ _xyzw, _xyzw, _xyzw, _xyzw }}); })
# define _V4IOF0() ((v4i){{ 0, 0, 0, 0 }})
# define v4i_of(...) VMACRO(_V4IOF, __VA_ARGS__)

#endif