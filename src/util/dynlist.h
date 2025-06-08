#ifndef UTIL_DYNLIST_H
# define UTIL_DYNLIST_H

# include <stdlib.h>

typedef struct {
	void *data;
	int size;
	int capacity;
	size_t data_size;
}	dynlist_t;

void	dynlist_create(dynlist_t *list, int capacity, size_t data_size);
void	dynlist_destroy(dynlist_t *list);
void	dynlist_clear(dynlist_t *list);
void	dynlist_append(dynlist_t *list, void *data);

// Return total size in byte of dynlist
static inline size_t dynlist_bytesize(dynlist_t l) {
	return (l.size * l.data_size);
}

static inline int dynlist_size(dynlist_t l) {
	return l.size;
}

static inline int dynlist_capacity(dynlist_t l) {
	return l.capacity;
}

static inline void *dynlist_data(dynlist_t l) {
	return l.data;
}

# define dynlist_foreach(list_ptr, ...) \
	do { \
		dynlist_t *l = list_ptr; \
		for (int index = 0; index < l->size; index++) { \
			void *data = (char*)l->data + index * l->data_size; \
			__VA_ARGS__ \
		}; \
	}	while (0);

#endif