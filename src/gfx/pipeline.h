#ifndef GFX_PIPELINE_H
# define GFX_PIPELINE_H

# include "buffer.h"
# include "shader.h"
# include "../util/macros.h"

typedef enum {
	PIPELINE_ARRAYS = 1,
	PIPELINE_ELEMENTS,
	PIPELINE_ELEMENTS_INSTANCED,
}	pipeline_render_method_e;

typedef enum {
	PIPELINE_TRIANGLES = GL_TRIANGLES,
}	pipeline_render_type_e;

typedef enum {
	PIPELINE_UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	PIPELINE_UNSIGNED_INT = GL_UNSIGNED_INT,
}	pipeline_elements_type_e;

typedef struct {
	buffer_type_e type;
	buffer_usage_e usage;
	buffer_steptype_e steptype;
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
	pipeline_render_method_e method;
	pipeline_render_type_e type;

	unsigned int element_count;
	pipeline_elements_type_e element_type;
}	pipeline_render_desc;

typedef struct {
	buffer_t vao;
	shader_t shader;

	int buffer_count;
	struct {
		buffer_t buffer;
		size_t stride;
		buffer_steptype_e steptype;
	}	*buffers;

	struct {
		pipeline_render_method_e method;
		pipeline_render_type_e type;

		unsigned int e_count;
		pipeline_elements_type_e e_type;
	}	render;
}	pipeline_t;

// Call pipeline bind
void	pipeline_create(pipeline_t *pipeline, shader_t shader, int buffer_count, pipeline_buffers_desc *buffers);
void	pipeline_destroy(pipeline_t *pipeline);
void	pipeline_bind(pipeline_t pipeline);
bool	pipeline_valid(pipeline_t pipeline);
void	pipeline_attributes_assign(pipeline_t *pipeline, int attributes_count, pipeline_attributes_desc *attributes);
void	pipeline_render_setup(pipeline_t *pipeline, pipeline_render_desc render_method);
void	_pipeline_render(pipeline_t pipeline, unsigned int count, size_t offset);

# define pipeline_subdata(pip_ptr, buf_idx, off, size, data) \
	buffer_subdata(&(pip_ptr)->buffers[buf_idx].buffer, off, size, data)

# define _PIPRENDER3(pip, count, off) _pipeline_render(pip, count, offset)
# define _PIPRENDER2(pip, count) _pipeline_render(pip, count, 0)
# define _PIPRENDER1(pip) _pipeline_render(pip, 1, 0)
/**
 * pipeline
 * count (default: 1)
 * offset (default: 0)
 **/
# define pipeline_render(...) VMACRO(_PIPRENDER, __VA_ARGS__)

#endif