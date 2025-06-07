#include "pipeline.h"

#include "../util/assert.h"

#include <stdlib.h>

typedef enum {
	PIPELINE_MAX_VERTEX_ATTRIB_COUNT,
	PIPELINE_LIMIT_COUNT,
}	pipeline_limit_e;

static int pipeline_limits_get[PIPELINE_LIMIT_COUNT];

static bool pipeline_limits_set;
static void pipeline_limits_init()
{
	if (pipeline_limits_set)
		return ;
	pipeline_limits_set = true;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &pipeline_limits_get[PIPELINE_MAX_VERTEX_ATTRIB_COUNT]);
}

void	pipeline_create(pipeline_t *pipeline, shader_t shader, int buffer_count, pipeline_buffers_desc *buffers_desc)
{
	*pipeline = (pipeline_t){
		.buffer_count = buffer_count,
		.shader = shader,
	};

	buffer_create(&pipeline->vao, BUFFER_VERTEX_ARRAY);
	if (buffer_count > 0);	
		pipeline->buffers = malloc(sizeof(*pipeline->buffers) * buffer_count);
	for (int i = 0; i < buffer_count; i++)
	{
		buffer_create(&pipeline->buffers[i].buffer, buffers_desc[i].type, buffers_desc[i].usage);
		buffer_data(&pipeline->buffers[i].buffer, buffers_desc[i].size, buffers_desc[i].data);
		pipeline->buffers[i].stride = buffers_desc[i].stride;
		pipeline->buffers[i].steptype = buffers_desc[i].steptype;
	}
}

void	pipeline_destroy(pipeline_t *pipeline)
{
	for (int i = 0; i < pipeline->buffer_count; i++)
		buffer_destroy(&pipeline->buffers[i].buffer);
	if (pipeline->buffer_count > 0)
		free(pipeline->buffers);
	buffer_destroy(&pipeline->vao);

	*pipeline = (pipeline_t){0};
}

void	pipeline_bind(pipeline_t pipeline)
{
	shader_bind(pipeline.shader);
	buffer_bind(pipeline.vao);
}

bool	pipeline_valid(pipeline_t pipeline)
{
	if (!buffer_valid(pipeline.vao) || !shader_valid(pipeline.shader))
		return false;

	for (int i = 0; i < pipeline.buffer_count; i++)
		if (!buffer_valid(pipeline.buffers[i].buffer))
			return false;

	return true;
}

void	pipeline_attributes_assign(pipeline_t *pipeline, int attributes_count, pipeline_attributes_desc *attributes)
{
	pipeline_limits_init();

	ASSERT(attributes_count < pipeline_limits_get[PIPELINE_MAX_VERTEX_ATTRIB_COUNT],
		"Reached max vertex attribs per pipeline (limit: %s)",
		pipeline_limits_get[PIPELINE_MAX_VERTEX_ATTRIB_COUNT]);

	for (unsigned int i = 0; i < attributes_count; i++)
	{
		int buf_index = attributes[i].buffer_index;

		buffer_bind(pipeline->buffers[buf_index].buffer);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			attributes[i].size,
			attributes[i].datatype,
			attributes[i].normalized ? GL_TRUE : GL_FALSE,
			pipeline->buffers[buf_index].stride,
			(void*)attributes[i].offset);
		glVertexAttribDivisor(i, pipeline->buffers[buf_index].steptype);
	}
}

void	pipeline_render_setup(pipeline_t *pipeline, pipeline_render_desc render_method)
{
	pipeline->render.method = render_method.method;
	pipeline->render.type = render_method.type;
	pipeline->render.e_count = render_method.element_count;
	pipeline->render.e_type = render_method.element_type;
}

void	_pipeline_render(pipeline_t pipeline, unsigned int count, size_t offset)
{
	ASSERT(pipeline.render.method,
		"Render method not set for pipeline.");

	switch (pipeline.render.method) {
		case (PIPELINE_ARRAYS):
			glDrawArrays(pipeline.render.type, offset, count);
			break ;
		case (PIPELINE_ELEMENTS):
			glDrawElements(
				pipeline.render.type, pipeline.render.e_count, 
				pipeline.render.e_type, (void*)offset);
			break ;
		case (PIPELINE_ELEMENTS_INSTANCED):
			glDrawElementsInstanced(
				pipeline.render.type, pipeline.render.e_count,
				pipeline.render.e_type, (void*)offset, count);
			break ;
		default:
			ASSERT(false, "unknow render method");
	};
}
