#include "sprite.h"

#define SPRITE_MAXCOUNT_PER_FRAME 65356

typedef struct {
	v2 position;
	v2 texcoord;
}	sprite_vertex_t;

typedef struct {
	v4 color;
	v2 offset;
	v2 scale;
	float z;
}	sprite_instance_t;

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
	pipeline_attributes_assign(&pipeline, 6, 
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

	manager->vs_params_ubuff_index = 0;
	manager->instance_buffer_index = 1;
	manager->shader = shader;
	manager->pipeline = pipeline;
}

void	sprite_manager_destroy(sprite_manager_t *manager)
{
	dynlist_destroy(&manager->sprites_list);

	pipeline_destroy(&manager->pipeline);
	shader_destroy(&manager->shader);

	*manager = (sprite_manager_t){0};
}

void	sprite_manager_push(sprite_manager_t *manager, sprite_t sprite)
{
	dynlist_append(&manager->sprites_list, 
		&(sprite_instance_t){
			.color = sprite.color,
			.offset = sprite.pos,
			.z = sprite.z,
			.scale = v2_of(8), // SPRITE ATLAS RELATED : TODO
		});
}

void	sprite_manager_draw(sprite_manager_t *manager)
{
	if (!dynlist_size(manager->sprites_list))
		return ;

	pipeline_subdata(
		&manager->pipeline,
		manager->instance_buffer_index,
		0,
		dynlist_bytesize(manager->sprites_list),
		dynlist_data(manager->sprites_list));
	pipeline_render(
		manager->pipeline,
		dynlist_size(manager->sprites_list));

	dynlist_clear(&manager->sprites_list);
}
