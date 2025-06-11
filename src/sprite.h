#ifndef SPRITE_H
# define SPRITE_H

# include "gfx/color.h"
# include "gfx/texture.h"
# include "gfx/pipeline.h"
# include "math/aliases.h"
# include "util/dynlist.h"

typedef enum {
	SPRITE_RGBA,
	SPRITE_FORMAT_COUNT,
}	sprite_atlas_format_e;

typedef struct {
	const char *path;
	v2i sprite_size;
	sprite_atlas_format_e format, internal_format;
}	sprite_atlas_desc;

typedef struct {
	texture_t tex;

	v2i sprite_size;
	v2i size_in_sprites;
	v2 sprite_size_tx;
	v2 tx_per_px;
}	sprite_atlas_t;

typedef struct {
	sprite_atlas_t tex_atlas;
	v2 pos;
	float z;
	color color;
	v2i	index;
	v2 scale;
}	sprite_t;

typedef struct {
	pipeline_t pipeline;
	shader_t shader;

	int instance_buffer_index;
	int vs_params_ubuff_index;
	dynlist_t sprites_list;

	int atlascount;
	sprite_atlas_t *atlas;
	unsigned int atlasloc;
}	sprite_manager_t;

void	sprite_manager_create(sprite_manager_t *manager, unsigned int vs_params_bp);
// Destroy registered sprite_atlas
void	sprite_manager_destroy(sprite_manager_t *manager);
void	sprite_manager_push(sprite_manager_t *manager, sprite_t sprite);
void	sprite_manager_draw(sprite_manager_t *manager);
void	sprite_manager_register(sprite_manager_t *manager, sprite_atlas_t atlas);
void	sprite_atlas_create(sprite_atlas_t *atlas, sprite_atlas_desc atlas_desc);
void	sprite_atlas_destroy(sprite_atlas_t *atlas);

#endif