#include "sprite.h"
#include "util/dynlist.h"

#ifndef STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb/stb_image.h>
#include "util/assert.h"

#define SPRITE_MAXCOUNT_PER_FRAME 65356
#define SPRITE_TEXATLAS_MAX_COUNT 8 // See sprite.frag

typedef struct {
	v2 position;
	v2 texcoord;
}	sprite_vertex_t;

typedef struct {
	v4 color;
	v2 offset;
	v2 scale;
	float z;
	int texindex;
	v2 uv_min;
	v2 uv_max;
}	sprite_instance_t;

static unsigned int sprite_target_compcount[SPRITE_FORMAT_COUNT] = {
	4, // RGBA
};

static unsigned int sprite_target_format[SPRITE_FORMAT_COUNT] = {
	TEXTURE_RGBA, // RGBA
};

void	sprite_manager_create(sprite_manager_t *manager, unsigned int vs_params_bp)
{
	*manager = (sprite_manager_t){0};

	shader_t shader;
	pipeline_t pipeline;

	unsigned short indices[] = { 2, 1, 0, 3, 2, 0};
	sprite_vertex_t vertices[] = {
		{ v2_of(0, 1), v2_of(0, 1) },
		{ v2_of(1, 1), v2_of(1, 1) },
		{ v2_of(1, 0), v2_of(1, 0) },
		{ v2_of(0, 0), v2_of(0, 0) },
	};

	shader_create(&shader, "res/shaders/sprite.vert", "res/shaders/sprite.frag");
	shader_uniform_setbindpoint(shader, "vs_params", vs_params_bp);
	shader_uniform_loc(shader, "uTextures", &manager->atlasloc);
	ASSERT(shader_valid(shader));

	pipeline_create(&pipeline, shader, 3, 
		(pipeline_buffers_desc[]){
			{
				.type = BUFFER_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(vertices),
				.stride = sizeof(sprite_vertex_t),
				.data = vertices
			},
			{
				.type = BUFFER_ARRAY,
				.usage = BUFFER_STREAM_DRAW,
				.size = sizeof(sprite_instance_t) * SPRITE_MAXCOUNT_PER_FRAME,
				.stride = sizeof(sprite_instance_t),
				.steptype = BUFFER_PER_INSTANCE
			},
			{
				.type = BUFFER_ELEMENT_ARRAY,
				.usage = BUFFER_STATIC_DRAW,
				.size = sizeof(indices),
				.data = indices,
			}
		});
	pipeline_attributes_assign(&pipeline, 9, 
		(pipeline_attributes_desc[]){
			{
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_vertex_t, position),
			},
			{
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_vertex_t, texcoord),
			},
			{
				.buffer_index = 1,
				.size = 4,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, color),
			},
			{
				.buffer_index = 1,
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, offset),
			},
			{
				.buffer_index = 1,
				.size = 2,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, scale),
			},
			{
				.buffer_index = 1,
				.size = 1,
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, z),
			},
			{
				.buffer_index = 1,
				.size = 1,
				.datatype = BUFFER_INT,
				.offset = offsetof(sprite_instance_t, texindex),
			},
			{
				.buffer_index = 1,
				.size = 2, 
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, uv_min)
			},
			{
				.buffer_index = 1,
				.size = 2, 
				.datatype = BUFFER_FLOAT,
				.offset = offsetof(sprite_instance_t, uv_max)
			},
		});
	pipeline_render_setup(&pipeline, 
		(pipeline_render_desc){
			.method = PIPELINE_ELEMENTS_INSTANCED,
			.type = PIPELINE_TRIANGLES,
			.element_count = 6,
			.element_type = PIPELINE_UNSIGNED_SHORT
		});
	ASSERT(pipeline_valid(pipeline));

	dynlist_create(&manager->sprites_list, SPRITE_MAXCOUNT_PER_FRAME, sizeof(sprite_instance_t));

	manager->atlas = malloc(sizeof(sprite_atlas_t) * SPRITE_TEXATLAS_MAX_COUNT);

	manager->vs_params_ubuff_index = 0;
	manager->instance_buffer_index = 1;
	manager->shader = shader;
	manager->pipeline = pipeline;
}

void	sprite_manager_destroy(sprite_manager_t *manager)
{
	for (int i = 0; i < manager->atlascount; i++)
		sprite_atlas_destroy(&manager->atlas[i]);
	free(manager->atlas);

	dynlist_destroy(&manager->sprites_list);

	pipeline_destroy(&manager->pipeline);
	shader_destroy(&manager->shader);

	*manager = (sprite_manager_t){0};
}

void	sprite_manager_push(sprite_manager_t *manager, sprite_t sprite)
{
	bool registered_tex_atlas = false;

	for (int i = 0; i < SPRITE_TEXATLAS_MAX_COUNT; i++)
	{
		if (manager->atlas[i].tex.id == sprite.tex_atlas.tex.id)
		{
			registered_tex_atlas = true;
			break ;
		}
	}

	ASSERT(registered_tex_atlas, "tex atlas not registered in sprite manager");
	ASSERT(sprite.index.x < sprite.tex_atlas.size_in_sprites.x
		&& sprite.index.y < sprite.tex_atlas.size_in_sprites.y,
		"Index out of bound");

	v2 uv_min = 
		v2_mul(
			v2_from_v(
				v2i_mul(sprite.index, sprite.tex_atlas.sprite_size)),
			sprite.tex_atlas.tx_per_px);
	v2 uv_max = 
		v2_add(uv_min, sprite.tex_atlas.sprite_size_tx);

	dynlist_append(&manager->sprites_list, 
		&(sprite_instance_t){
			.color = sprite.color,
			.offset = sprite.pos,
			.z = sprite.z,
			.scale = v2_mul(v2_from_v(sprite.tex_atlas.sprite_size), sprite.scale),
			.texindex = sprite.tex_atlas.tex.bind_point,
			.uv_min = uv_min,
			.uv_max = uv_max,
		});
}

void	sprite_manager_draw(sprite_manager_t *manager)
{
	unsigned int dsize = dynlist_size(manager->sprites_list);

	if (!dsize)
		return ;

	for (int i = 0; i < manager->atlascount; i++)
		texture_bind(manager->atlas[i].tex);

	unsigned int count = min(dsize, SPRITE_MAXCOUNT_PER_FRAME);
	if (count != dsize)
		fprintf(stderr, "REACHED SPRITES LIMIT (limit: %u, drawing %d)\n", 
			SPRITE_MAXCOUNT_PER_FRAME, dsize);

	pipeline_bind(manager->pipeline);
	pipeline_subdata(
		&manager->pipeline,
		manager->instance_buffer_index,
		0,
		count * manager->sprites_list.data_size,
		dynlist_data(manager->sprites_list));
	pipeline_render(
		manager->pipeline,
		count);

	dynlist_clear(&manager->sprites_list);
}

void	sprite_manager_register(sprite_manager_t *manager, sprite_atlas_t atlas)
{
	static unsigned int atlasbps[SPRITE_TEXATLAS_MAX_COUNT];

	ASSERT(manager->atlascount < SPRITE_TEXATLAS_MAX_COUNT,
		"Reached max amount of texatlas");

	pipeline_bind(manager->pipeline);

	atlasbps[manager->atlascount] = atlas.tex.bind_point;

	manager->atlas[manager->atlascount] = atlas;
	manager->atlascount++;

	glUniform1iv(manager->atlasloc, manager->atlascount, (const int *)atlasbps);
}

void	sprite_atlas_create(sprite_atlas_t *atlas, sprite_atlas_desc atlas_desc)
{
	sprite_atlas_t new_tatlas = {
		.sprite_size = atlas_desc.sprite_size,
	};

	texture_create(&new_tatlas.tex, TEXTURE_2D);
	texture_sampler_set(&new_tatlas.tex,
		(texture_sampler_desc){
			.wrap_u = TEXTURE_CLAMP_TO_EDGE,
			.wrap_v = TEXTURE_CLAMP_TO_EDGE,
			.min_filter = TEXTURE_NEAREST,
			.max_filter = TEXTURE_NEAREST,
		});

	v2i tex_size;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = 
		stbi_load(
			atlas_desc.path, 
			&tex_size.x, &tex_size.y, NULL, 
			sprite_target_compcount[atlas_desc.format]);
	ASSERT(data,
		"Failed to load %s", atlas_desc.path);
	texture_data_set(&new_tatlas.tex,
		(texture_data_desc){
			.size = tex_size,
			.internal_format = sprite_target_format[atlas_desc.internal_format],
			.format = sprite_target_format[atlas_desc.format],
			.datatype = TEXTURE_UNSIGNED_BYTE,
			.data = data,
		});
	ASSERT(texture_valid(new_tatlas.tex));

	free(data);

	ASSERT(new_tatlas.tex.size.x % new_tatlas.sprite_size.x == 0, 
		"Sprite atlas dimension isn't valid on x axis");
	ASSERT(new_tatlas.tex.size.y % new_tatlas.sprite_size.y == 0,
		"Sprite atlas dimension isn't valid on y axis");

	new_tatlas.size_in_sprites = v2i_div(tex_size, new_tatlas.sprite_size);
	new_tatlas.tx_per_px = v2_div(v2_of(1), v2_from_v(tex_size));
	new_tatlas.sprite_size_tx = 
		v2_mul(v2_from_v(new_tatlas.sprite_size), new_tatlas.tx_per_px);

	ASSERT(texture_valid(new_tatlas.tex), 
		"failed to create new tex atlas %s", atlas_desc.path);

	*atlas = new_tatlas;
}

void	sprite_atlas_destroy(sprite_atlas_t *atlas)
{
	texture_destroy(&atlas->tex);

	*atlas = (sprite_atlas_t){0};
}
