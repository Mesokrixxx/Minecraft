#include "ui.h"
#include "util/camera.h"

typedef struct {
	mat4 model;
	mat4 view;
	mat4 proj;
}	vs_params_t;

void	ui_init(ui_t *ui, window_t *window, vs_params_ubo_desc vsparam_ubo)
{
	*ui = (ui_t){
		.model = vsparam_ubo.model,
		.ubobuf = vsparam_ubo.buffer,
	};

	camera_init(&ui->cam,
		(camera_desc){
			.type = CAMERA_ORTHOGRAPHIC,
			.window = window,
			.zfar = 1.0,
			.znear = -1.0,
			.orthographic = (camera_orthographic_desc){
				.min = v2_of(0),
				.max = v2_from_v(window->size),
			}
		});
}

void	ui_event_windowresize(ui_t *ui)
{
	ui->cam.ortho.max = v2_from_v(ui->cam.window->size);
	camera_update(&ui->cam);
}

void	ui_pass(ui_t ui)
{
	vs_params_t params;
	memcpy(params.model, ui.model, sizeof(*ui.model));
	memcpy(params.view, &ui.cam.view, sizeof(ui.cam.view));
	memcpy(params.proj, &ui.cam.projection, sizeof(ui.cam.projection));
	buffer_subdata(ui.ubobuf, 0, sizeof(vs_params_t), &params);
}
