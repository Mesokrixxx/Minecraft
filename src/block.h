#ifndef BLOCK_H
# define BLOCK_H

# include "gfx/shader.h"
# include "gfx/pipeline.h"
# include "sprite.h"
# include "util/dynlist.h"
# include "math/aliases.h"

typedef enum {
	BLOCK_AIR = -1,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_TYPE_COUNT,
}	block_type_e;

typedef struct {
	block_type_e type;
	v2 pos;
	float z;
}	block_desc;

typedef struct {
	shader_t shader;
	pipeline_t pipeline;
	sprite_atlas_t block_faces_atlas;
	dynlist_t blocks;
	unsigned int instbuf;
	unsigned int blockatlas_uniformloc;
}	blocks_manager_t;

void	blocks_manager_init(blocks_manager_t *manager, unsigned int vsparams_bp);
void	blocks_manager_push(blocks_manager_t *manager, block_desc block);
void	blocks_manager_render(blocks_manager_t *manager);
void	blocks_manager_destroy(blocks_manager_t *manager);

#endif