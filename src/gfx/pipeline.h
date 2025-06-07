#ifndef GFX_PIPELINE_H
# define GFX_PIPELINE_H

# include "buffer.h"
# include "shader.h"

typedef struct {
	buffer_type_e type;
	buffer_usage_e usage;
	size_t size;
	size_t stride;
	const void *data;
}	pipeline_buffers_desc;

typedef struct {
	unsigned int buffer_index;
	int size;
	buffer_datatype_e datatype;
	bool normalized;
	size_t offset;
}	pipeline_attributes_desc;

typedef struct {
	buffer_t vao;
	shader_t shader;

	int buffer_count;
	struct {
		buffer_t buffer;
		size_t stride;
	}	*buffers;
}	pipeline_t;

void	pipeline_create(pipeline_t *pipeline, shader_t shader, int buffer_count, pipeline_buffers_desc *buffers);
void	pipeline_destroy(pipeline_t *pipeline);
void	pipeline_bind(pipeline_t pipeline);
bool	pipeline_valid(pipeline_t pipeline);
void	pipeline_assign_attributes(pipeline_t *pipeline, int attributes_count, pipeline_attributes_desc *attributes);

#endif