#include "sparseset.h"

#include "assert.h"

#include <string.h>

void	sparseset_create(sparseset_t *ss, int capacity, size_t datasize)
{
	*ss = (sparseset_t){
		.capacity = capacity,
		.sparse_cap = capacity,
		.datasize = datasize,
	};

	ss->data = malloc(ss->capacity * ss->datasize);
	ss->dense = malloc(ss->capacity * sizeof(int));
	ss->sparse = malloc(ss->sparse_cap * sizeof(int));
	memset(ss->sparse, -1, ss->sparse_cap * sizeof(int));
}

void	sparseset_destroy(sparseset_t *ss)
{
	free(ss->data);
	free(ss->dense);
	free(ss->sparse);
	*ss = (sparseset_t){0};
}

void	_sparseset_add(sparseset_t *ss, void *data, int id)
{
	ASSERT(!sparseset_contains(*ss, id),
		"given id is already registered (%d)", id);

	if (id >= ss->sparse_cap)
	{
		int offset = ss->sparse_cap;
		int new_cap = ss->sparse_cap * 2;
		if (id > new_cap)
			new_cap = id + 1;
		ss->sparse_cap = new_cap;
		ss->sparse = realloc(ss->sparse, new_cap * sizeof(int));
		memset(((char *)ss->sparse + offset * sizeof(int)), -1, (new_cap - offset) * sizeof(int));
	}

	if (ss->size >= ss->capacity)
	{
		ss->capacity *= 2;
		ss->dense = realloc(ss->dense, ss->capacity * sizeof(int));
		ss->data = realloc(ss->data, ss->capacity * ss->datasize);
	}

	ss->sparse[id] = ss->size;
	ss->dense[ss->size] = id;
	memcpy(((char*)ss->data + ss->size * ss->datasize), data, ss->datasize);
	ss->size++;
}

void	sparseset_remove(sparseset_t *ss, int id)
{
	ASSERT(sparseset_contains(*ss, id),
		"id isn't registered (%d)", id);

	int index = ss->sparse[id];
	if (index != ss->size - 1)
	{
		int last_id = ss->dense[ss->size - 1];

		ss->sparse[last_id] = index;
		ss->dense[index] = last_id;
		memcpy(
			((char*)ss->data + index * ss->datasize), 
			((char*)ss->data + (ss->size - 1) * ss->datasize), 
			ss->datasize);
	}

	ss->sparse[id] = -1;
	ss->size--;
}

void	*sparseset_get(sparseset_t *ss, int id)
{
	if (sparseset_contains(*ss, id))
		return ((char*)ss->data + ss->sparse[id] * ss->datasize);
	return NULL;
}

bool	sparseset_contains(sparseset_t ss, int id)
{
	if (id >= 0 && id <= ss.sparse_cap)
		if (ss.sparse[id] != -1)
			return true;
	return false;
}
