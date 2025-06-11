#ifndef MATH_ALIASES_H
# define MATH_ALIASES_H

# include <cglm/types-struct.h>
# include <cglm/struct/vec2.h>
# include <cglm/struct/ivec2.h>
# include <cglm/struct/vec3.h>
# include <cglm/struct/ivec3.h>
# include <cglm/struct/vec4.h>
# include <cglm/struct/ivec4.h>
# include <cglm/struct/mat3.h>
# include <cglm/struct/mat4.h>
# include <cglm/struct/cam.h>
# include <cglm/struct/affine.h>

# include "../util/macros.h"
# include "vars.h"

# define v2_add glms_vec2_add
# define v2_sub glms_vec2_sub
# define v2_mul glms_vec2_mul
# define v2_div glms_vec2_div

# define v2i_add glms_ivec2_add
# define v2i_mul glms_ivec2_mul
# define v2i_div glms_ivec2_div

# define v3_add glms_vec3_add
# define v3_normalize glms_vec3_normalize
# define v3_cross glms_vec3_cross
# define v3_scale glms_vec3_scale

# define m3_identity glms_mat3_identity

# define m4_identity glms_mat4_identity
# define m4_rotate_y glms_rotate_y
# define m4_rotate_x glms_rotate_x
# define m4_rotate_z glms_rotate_z

# define cam_ortho glms_ortho
# define cam_lookat glms_lookat
# define cam_perspective glms_perspective

typedef vec2s	v2;
typedef ivec2s	v2i;
typedef vec3s	v3;
typedef ivec3s	v3i;
typedef vec4s	v4;
typedef ivec4s	v4i;
typedef mat3s	m3;
typedef mat4s	m4;

# define _V2OF2(x, y) ((v2){{ x, y }})
# define _V2OF1(xy)	({ float _xy = (xy); ((v2){{ _xy, _xy }}); })
# define _V2OF0() ((v2){{ 0, 0 }})
# define v2_of(...) VMACRO(_V2OF, __VA_ARGS__)
# define v2_from_v(v) ({ __typeof__(v) _v = (v); ((v2){{ _v.x, _v.y }}); })

# define _V2IOF2(x, y) ((v2i){{ x, y }})
# define _V2IOF1(xy) ({ int _xy = (xy); ((v2i){{ _xy, _xy }}); })
# define _V2IOF0() ((v2i){{ 0, 0 }})
# define v2i_of(...) VMACRO(_V2IOF, __VA_ARGS__)
# define v2i_from_v(v) ({ __typeof__(v) _v = (v); ((v2i){{ _v.x, _v.y }}); })

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
# define _V4IOF1(xyzw) ({ int _xyzw = (xyzw); ((v4i){{ _xyzw, _xyzw, _xyzw, _xyzw }}); })
# define _V4IOF0() ((v4i){{ 0, 0, 0, 0 }})
# define v4i_of(...) VMACRO(_V4IOF, __VA_ARGS__)

static inline v2i v2i_clampv(v2i origin, v2i min, v2i max) {
	return (v2i_of(clamp(origin.x, min.x, max.x), clamp(origin.y, min.y, max.y)));
}

#endif