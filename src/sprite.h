#ifndef SPRITE_H
# define SPRITE_H

# include "gfx/gfx.h"
# include "util/util.h"
# include "math/math.h"

typedef struct {
	v2 pos;
	float z;
	color color;
}	sprite_t;

typedef struct {

}	sprite_atlas_t;

typedef struct {
	pipeline_t pipeline;
	shader_t shader;

	int instance_buffer_index;
	int vs_params_ubuff_index;
	dynlist_t sprites_list;
}	sprite_manager_t;

void	sprite_manager_create(sprite_manager_t *manager, unsigned int vs_params_bp);
void	sprite_manager_destroy(sprite_manager_t *manager);
void	sprite_manager_push(sprite_manager_t *manager, sprite_t sprite);
void	sprite_manager_draw(sprite_manager_t *manager);

#endif