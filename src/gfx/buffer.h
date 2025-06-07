#ifndef GFX_BUFFER_H
# define GFX_BUFFER_H

# include <stdbool.h>
# include <GL/glew.h>

# include "../util/macros.h"

typedef enum {
	BUFFER_ARRAY,
	BUFFER_ELEMENT_ARRAY,
	BUFFER_VERTEX_ARRAY,
	BUFFER_UNIFORM,
	BUFFER_TYPE_COUNT,
}	buffer_type_e;

typedef enum {
	BUFFER_NULL_USAGE = 0, // Can't use buffer_data
	BUFFER_STATIC_DRAW = GL_STATIC_DRAW,
	BUFFER_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
	BUFFER_STREAM_DRAW = GL_STREAM_DRAW,
}	buffer_usage_e;

typedef enum {
	BUFFER_PER_VERTEX,
	BUFFER_PER_INSTANCE,
}	buffer_steptype_e;

typedef enum {
	BUFFER_FLOAT = GL_FLOAT,
}	buffer_datatype_e;

typedef struct {
	unsigned int id;
	buffer_type_e type;
	buffer_usage_e usage;
	size_t size;
}	buffer_t;

// Bind created buffer
void	_buffer_create(buffer_t *buffer, buffer_type_e type, buffer_usage_e usage);
void	buffer_data(buffer_t *buffer, size_t size, const void *data);
void	buffer_subdata(buffer_t *buffer, size_t offset, size_t size, const void *data);
void	buffer_destroy(buffer_t *buffer);
void	buffer_bind(buffer_t buffer);
bool	buffer_valid(buffer_t buffer);
void	buffer_bindpoint_assign(buffer_t buffer, unsigned int *bind_point);

# define _BUFCREATE3(buffer_ptr, type, usage) _buffer_create(buffer_ptr, type, usage)
# define _BUFCREATE2(buffer_ptr, type) _buffer_create(buffer_ptr, type, BUFFER_NULL_USAGE)
/**
 * buffer_ptr
 * type
 * usage (default: BUFFER_NULL_USAGE)
 **/
# define buffer_create(...) VMACRO(_BUFCREATE, __VA_ARGS__)

#endif