#ifndef UTIL_SPARSESET_H
# define UTIL_SPARSESET_H

# include "../util/macros.h"

# include <stdlib.h>
# include <stdbool.h>

typedef struct {
	void *data;
	int *sparse, *dense;
	int size;
	int capacity, sparse_cap;
	size_t datasize; 
}	sparseset_t;

void	sparseset_create(sparseset_t *ss, int capacity, size_t datasize);
void	sparseset_destroy(sparseset_t *ss);
void	_sparseset_add(sparseset_t *ss, void *data, int id);
void	sparseset_remove(sparseset_t *ss, int id);
void	*sparseset_get(sparseset_t *ss, int id);
bool	sparseset_contains(sparseset_t ss, int id);

# define _SSADD3(ss_ptr, data, id) _sparseset_add(ss_ptr, data, id)
# define _SSADD2(ss_ptr, data) _sparseset_add(ss_ptr, data, (ss_ptr)->size)
/**
 * sparseset ptr
 * data
 * id (default: next free id in sparseset)
 **/
# define sparseset_add(...) VMACRO(_SSADD, __VA_ARGS__)

# define sparseset_foreach(ss_ptr, ...) \
	do { \
		sparseset_t *ss; \
		for (int index; index < ss->size; index++) { \
			int id = ss->dense[index]; \
			void *data = ss->data + index * ss->datasize; \
			__VA_ARGS__ \
		} \
	} while (0);

#endif