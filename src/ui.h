#ifndef UI_H
# define UI_H

# include "gfx/buffer.h"
# include "util/camera.h"

typedef struct {
	buffer_t *buffer;
	m4 *model;
}	vs_params_ubo_desc;

typedef struct {
	camera_t	cam;
	
	m4 *model;
	buffer_t *ubobuf;
}	ui_t;

void	ui_init(ui_t *ui, window_t *window, vs_params_ubo_desc vsparam_ubo);
void	ui_event_windowresize(ui_t *ui);
void	ui_pass(ui_t ui);

#endif