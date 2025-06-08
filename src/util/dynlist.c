#include "dynlist.h"

#include <string.h>

void	dynlist_create(dynlist_t *list, int capacity, size_t data_size)
{
	*list = (dynlist_t){
		.capacity = capacity,
		.data_size = data_size,
	};

	list->data = malloc(list->data_size * list->capacity);
}

void	dynlist_destroy(dynlist_t *list)
{
	free(list->data);

	*list = (dynlist_t){0};
}

void	dynlist_append(dynlist_t *list, void *data)
{
	if (list->size == list->capacity)
	{
		list->capacity *= 2;
		list->data = realloc(list->data, list->capacity * list->data_size);
	}

	memcpy((char*)list->data + list->size * list->data_size, data, list->data_size);
	list->size++;
}

void dynlist_clear(dynlist_t *l) {
	l->size = 0; 
}
