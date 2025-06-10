#include "camera.h"

#include "assert.h"
#include "../math/vars.h"

#define PTICH_LIMIT (PI_2 - 0.1f)

void	camera_init(camera_t *camera, camera_desc desc)
{
	*camera = (camera_t){
		.type = desc.type,
		.window = desc.window,
		.znear = desc.znear,
		.zfar = desc.zfar,
	};

	switch (desc.type) {
		case (CAMERA_ORTHOGRAPHIC):
			camera->ortho.min = desc.orthographic.min;
			camera->ortho.max = desc.orthographic.max;
			break;
		case (CAMERA_PERSPECTIVE):
			camera->persp.fov = desc.perspective.fov;
			camera->persp.aspect = 
				((float)desc.window->size.x) / ((float)desc.window->size.y);
			break;
		default:
			ASSERT(false, "unknow camera type"); 
	};
	camera_update(camera);
}

void	camera_update(camera_t *camera)
{
	switch (camera->type) {
		case (CAMERA_ORTHOGRAPHIC):
			camera->view = m4_identity();
			camera->projection = 
				cam_ortho(
					camera->ortho.min.x,
					camera->ortho.max.x,
					camera->ortho.min.y,
					camera->ortho.max.y,
					camera->znear,
					camera->zfar);
			break;
		case (CAMERA_PERSPECTIVE):
			camera->persp.pitch = clamp(camera->persp.pitch, -PTICH_LIMIT, PTICH_LIMIT);
			camera->persp.yaw = 
				(camera->persp.yaw < 0 ? TAU : 0.0) + fmodf(camera->persp.yaw, TAU);
			camera->persp.dir = (v3) {{
				cosf(camera->persp.pitch) * sinf(camera->persp.yaw),
				sinf(camera->persp.pitch),
				cosf(camera->persp.pitch) * cosf(camera->persp.yaw),
			}};
			camera->persp.dir = v3_normalize(camera->persp.dir);
			camera->persp.right = v3_cross(v3_of(0, 1, 0), camera->persp.dir);
			camera->persp.up = v3_cross(camera->persp.dir, camera->persp.right);

			camera->view = 
				cam_lookat(
					camera->persp.pos, 
					v3_add(camera->persp.pos, camera->persp.dir), camera->persp.up);
			camera->projection = 
				cam_perspective(
					camera->persp.fov, 
					camera->persp.aspect, camera->znear, camera->zfar);
			break;
		default:
			ASSERT(false, "unknow camera type");
	};
}
