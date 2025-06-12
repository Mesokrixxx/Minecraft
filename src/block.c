#include "block.h"
#include "gfx/buffer.h"
#include "gfx/shader.h"
#include "math/aliases.h"
#include "util/assert.h"

#include <stdio.h>
#include <string.h>

#define MAX_CUBE_PERFRAME (65356 / 18)
#define BLOCK_FACE_SIZE 16

typedef struct {
	v3 pos;
	v2 texcoord;
	int faceid;
}	vertices_t;

typedef struct {
	v2 offset;
	float z;
	int blockid;
}	instances_t;

void	blocks_manager_init(blocks_manager_t *manager, unsigned int vsparams_bp)
{
	*manager = (blocks_manager_t){0};

	unsigned short indices[36];
	for (int i = 0; i < 6; i++) {
		int offset = i * 4;
		unsigned short face[] = {
			2 + offset, 1 + offset, 0 + offset,
			3 + offset, 2 + offset, 0 + offset
		};
		memcpy(indices + i * 6, face, sizeof(face));
	}
	vertices_t vertices[] = {
		// u
		{ v3_of(0, 1, -1), v2_of(0, 1), 0 },
		{ v3_of(1, 1, -1), v2_of(1, 1), 0 },
		{ v3_of(1, 1, 0), v2_of(1, 0), 0 },
		{ v3_of(0, 1, 0), v2_of(0, 0), 0 },
		// f
		{ v3_of(0, 1, 0), v2_of(0, 1), 1 },
		{ v3_of(1, 1, 0), v2_of(1, 1), 1 },
		{ v3_of(1, 0, 0), v2_of(1, 0), 1 },
		{ v3_of(0, 0, 0), v2_of(0, 0), 1 },
		// b
		{ v3_of(0, 1, -1), v2_of(0, 1), 2 },
		{ v3_of(1, 1, -1), v2_of(1, 1), 2 },
		{ v3_of(1, 0, -1), v2_of(1, 0), 2 },
		{ v3_of(0, 0, -1), v2_of(0, 0), 2 },
		// g
		{ v3_of(0, 1, -1), v2_of(0, 1), 3 },
		{ v3_of(0, 1, 0), v2_of(1, 1), 3 },
		{ v3_of(0, 0, 0), v2_of(1, 0), 3 },
		{ v3_of(0, 0, -1), v2_of(0, 0), 3 },
		// r
		{ v3_of(1, 1, 0), v2_of(0, 1), 4 },
		{ v3_of(1, 1, -1), v2_of(1, 1), 4 },
		{ v3_of(1, 0, -1), v2_of(1, 0), 4 },
		{ v3_of(1, 0, 0), v2_of(0, 0), 4 },
		// d
		{ v3_of(0, 0, 0), v2_of(0, 1), 5 },
		{ v3_of(1, 0, 0), v2_of(1, 1), 5 },
		{ v3_of(1, 0, -1), v2_of(1, 0), 5 },
		{ v3_of(0, 0, -1), v2_of(0, 0), 5 },
	};

	sprite_atlas_create(&manager->block_faces_atlas, 
		(sprite_atlas_desc){
			.path = "res/textures/tiles.png",
			.sprite_size = v2i_of(BLOCK_FACE_SIZE),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});
	shader_create(
		&manager->shader, 
		"res/shaders/blocks.vert", "res/shaders/blocks.frag");
	shader_uniform_setbindpoint(manager->shader, "vs_params", vsparams_bp);
	shader_uniform_loc(manager->shader, "BlockAtlas", &manager->blockatlas_uniformloc);
	glUniform1i(manager->blockatlas_uniformloc, manager->block_faces_atlas.tex.bind_point);
	pipeline_create(&manager->pipeline, manager->shader, 3,
		(pipeline_buffers_desc[]){
			{
				.type = BUFFER_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(vertices),
				.data = vertices,
				.stride = sizeof(vertices_t),
			},
			{
				.type = BUFFER_ARRAY,
				.usage = BUFFER_STREAM_DRAW,
				.size = sizeof(instances_t) * MAX_CUBE_PERFRAME,
				.stride = sizeof(instances_t),
				.steptype = BUFFER_PER_INSTANCE,
			},
			{
				.type = BUFFER_ELEMENT_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(indices),
				.data = indices,
			}
		});
	manager->instbuf = 1;
	pipeline_attributes_assign(&manager->pipeline, 6, 
		(pipeline_attributes_desc[]){
			{
				.size = 3,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(vertices_t, pos),
			},
			{
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(vertices_t, texcoord),
			},
			{
				.size = 1,
				.datatype = BUFFER_INT,
				.offset = offsetof(vertices_t, faceid),
			},
			{
				.buffer_index = 1,
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(instances_t, offset),
			},
			{
				.buffer_index = 1,
				.size = 1,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(instances_t, z),
			},
			{
				.buffer_index = 1,
				.size = 1,
				.datatype = BUFFER_INT,
				.offset = offsetof(instances_t, blockid),
			}
		});
	pipeline_render_setup(&manager->pipeline,
		(pipeline_render_desc){
			.method = PIPELINE_ELEMENTS_INSTANCED,
			.type = PIPELINE_TRIANGLES,
			.element_count = 36,
			.element_type = PIPELINE_UNSIGNED_SHORT,
		});
	dynlist_create(&manager->blocks, MAX_CUBE_PERFRAME, sizeof(instances_t));
}

void	blocks_manager_destroy(blocks_manager_t *manager)
{
	dynlist_destroy(&manager->blocks);
	pipeline_destroy(&manager->pipeline);
	shader_destroy(&manager->shader);

	*manager = (blocks_manager_t){0};
}

void	blocks_manager_push(blocks_manager_t *manager, block_desc block)
{
	if (block.type == BLOCK_AIR)
		return ;
	dynlist_append(&manager->blocks, 
		&(instances_t){
			.offset = block.pos,
			.z = block.z,
			.blockid = block.type,
		});
}

void	blocks_manager_render(blocks_manager_t *manager)
{
	unsigned int dsize = dynlist_size(manager->blocks);
	unsigned int count = min(dsize, MAX_CUBE_PERFRAME);

	if (!count)
		return ;

	if (count != dsize)
		fprintf(stderr, "REACHED CUBES LIMITS (limit: %u, drawing: %u)",
			MAX_CUBE_PERFRAME, dsize);

	texture_bind(manager->block_faces_atlas.tex);
	pipeline_bind(manager->pipeline);
	pipeline_subdata(
		&manager->pipeline,
		manager->instbuf,
		0,
		count * manager->blocks.data_size,
		dynlist_data(manager->blocks));
	pipeline_render(
		manager->pipeline,
		count);

	dynlist_clear(&manager->blocks);
}
