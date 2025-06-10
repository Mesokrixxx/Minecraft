#ifndef UTIL_CAMERA_H
# define UTIL_CAMERA_H

# include "../math/aliases.h"
# include "../gfx/window.h"

typedef enum {
	CAMERA_ORTHOGRAPHIC,
	CAMERA_PERSPECTIVE,
}	camera_type_e;

typedef struct {
	v2 min;
	v2 max;
}	camera_orthographic_desc;

typedef struct {
	float fov;
}	camera_perspective_desc;

typedef struct {
	camera_type_e type;
	window_t *window;

	float znear, zfar;
	union {
		camera_perspective_desc perspective;
		camera_orthographic_desc orthographic;
	};
}	camera_desc;

typedef struct {
	camera_type_e type;
	window_t *window;

	float znear, zfar;
	union {
		struct {
			v3 pos, dir, up, right;
			float pitch, yaw, fov, aspect;
		}	persp;
		struct {
			v2 min, max;
		}	ortho;
	};

	m4 projection;
	m4 view;
}	camera_t;

void	camera_init(camera_t *camera, camera_desc desc);
void	camera_update(camera_t *camera);

#endif