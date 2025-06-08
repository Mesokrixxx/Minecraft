#include "buffer.h"

#include "../util/assert.h"

#include <stdlib.h>

static unsigned int buffer_type_target[BUFFER_TYPE_COUNT] = {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	0, // BUFFER_VERTEX_ARRAY,
	GL_UNIFORM_BUFFER,
	GL_FRAMEBUFFER,
	GL_RENDERBUFFER,
};

void	_buffer_create(buffer_t *buffer, buffer_type_e type, buffer_usage_e usage)
{
	ASSERT(type < BUFFER_TYPE_COUNT,
		"unknow type");

	*buffer = (buffer_t){
		.type = type,
		.usage = usage,
	};

	switch (type) {
		case (BUFFER_VERTEX_ARRAY):
			glGenVertexArrays(1, &buffer->id);
			break ;
		case (BUFFER_FRAME):
			glGenFramebuffers(1, &buffer->id);
			break ;
		case (BUFFER_RENDER):
			glGenRenderbuffers(1, &buffer->id);
			break ;
		default:
			glGenBuffers(1, &buffer->id);
			break ;
	}
	buffer_bind(*buffer);
}

void	buffer_destroy(buffer_t *buffer)
{
	switch (buffer->type) {
		case (BUFFER_VERTEX_ARRAY):
			glDeleteVertexArrays(1, &buffer->id);
			break;
		case (BUFFER_FRAME):
			glDeleteFramebuffers(1, &buffer->id);
			break ;
		case (BUFFER_RENDER):
			glDeleteRenderbuffers(1, &buffer->id);
			break ;
		default:
			glDeleteBuffers(1, &buffer->id);
			break;
	}

	*buffer = (buffer_t){0};
}

void	buffer_bind(buffer_t buffer)
{
	static unsigned int lastbuffer[BUFFER_TYPE_COUNT];

	if (lastbuffer[buffer.type] != buffer.id)
	{
		switch (buffer.type) {
			case (BUFFER_VERTEX_ARRAY):
				glBindVertexArray(buffer.id);
				break;
			case (BUFFER_FRAME):
				glBindFramebuffer(buffer_type_target[buffer.type], buffer.id);
				break ;
			case (BUFFER_RENDER):
				glBindRenderbuffer(buffer_type_target[buffer.type], buffer.id);
				break ;
			default:
				glBindBuffer(buffer_type_target[buffer.type], buffer.id);
				break;
		}
		lastbuffer[buffer.type] = buffer.id;
	}
}

bool	buffer_valid(buffer_t buffer)
{
	buffer_bind(buffer);

	switch (buffer.type) {
		case (BUFFER_VERTEX_ARRAY):
			if (glIsVertexArray(buffer.id) != GL_TRUE)
				return false;
			break ;
		case (BUFFER_FRAME):
			if (glIsFramebuffer(buffer.id) != GL_TRUE
				&& glCheckFramebufferStatus(
					buffer_type_target[buffer.type]) != GL_FRAMEBUFFER_COMPLETE)
				return false;
			break ;
		case (BUFFER_RENDER):
			if (glIsRenderbuffer(buffer.id) != GL_TRUE)
				return false;
			break ;
		default:
			if (glIsBuffer(buffer.id) != GL_TRUE)
				return false;
			break ;
	}

	return true;
}

void	buffer_data(buffer_t *buffer, size_t size, const void *data)
{
	buffer_bind(*buffer);
	glBufferData(buffer_type_target[buffer->type], size, data, buffer->usage);

	buffer->size = size;
}

void	buffer_subdata(buffer_t *buffer, size_t offset, size_t size, const void *data)
{
	ASSERT(buffer->size >= size,
		"Buffer overflow (buffer size: %zu - Data size: %zu", buffer->size, size);

	if (!size)
		return ;

	buffer_bind(*buffer);
	glBufferSubData(buffer_type_target[buffer->type], offset, size, data);
}

void	buffer_bindpoint_assign(buffer_t buffer, unsigned int *bind_point)
{
	static unsigned int bind_points[BUFFER_TYPE_COUNT];

	unsigned int bp = bind_points[buffer.type];

	glBindBufferBase(buffer_type_target[buffer.type], bp, buffer.id);
	*bind_point = bp;
	bind_points[buffer.type]++;
}
