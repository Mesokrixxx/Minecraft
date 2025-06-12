#include "chunk.h"
#include "util/assert.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CHUNKCAP (8 * 8)
#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 16

#define INDEX(x, y, z) ((x) + CHUNK_WIDTH * ((y) + CHUNK_HEIGHT * (z)))

typedef struct {
	block_type_e *blocks;
	v3i pos;
}	chunk_t;

typedef struct {
	chunk_t *chunks;
	int chunks_count;
	int chunks_capacity;
}	chunks_manager_t;

static blocks_manager_t *BLOCKS_MANAGER;
static chunks_manager_t MANAGER;

void	chunks_init(blocks_manager_t *bmanager)
{
	BLOCKS_MANAGER = bmanager;

	MANAGER.chunks_capacity = DEFAULT_CHUNKCAP;
	MANAGER.chunks = malloc(sizeof(chunk_t) * MANAGER.chunks_capacity);
	for (int i = 0; i < MANAGER.chunks_capacity; i++)
		MANAGER.chunks[i].blocks = 
			malloc(sizeof(block_type_e) * CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT);
}

void	chunk_load(v3i pos)
{
	int chunk;

	chunk = 0;
	MANAGER.chunks[chunk].pos = pos;
	memset(MANAGER.chunks[chunk].blocks, BLOCK_AIR, sizeof(block_type_e) * CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT);
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < CHUNK_WIDTH; x++)
			for (int z = 0; z < CHUNK_DEPTH; z++)
				MANAGER.chunks[chunk].blocks[INDEX(x, y, z)] = BLOCK_STONE;
	for (int y = 4; y - 4 < 2; y++)
		for (int x = 0; x < CHUNK_WIDTH; x++)
			for (int z = 0; z < CHUNK_DEPTH; z++)
				MANAGER.chunks[chunk].blocks[INDEX(x, y, z)] = BLOCK_DIRT;
	for (int y = 6; y - 6 < 1; y++)
		for (int x = 0; x < CHUNK_WIDTH; x++)
			for (int z = 0; z < CHUNK_DEPTH; z++)
				MANAGER.chunks[chunk].blocks[INDEX(x, y, z)] = BLOCK_GRASS;
	
	MANAGER.chunks_count++;
	ASSERT(MANAGER.chunks_count < MANAGER.chunks_capacity, 
		"reached max cap for loaded chunks");
}

void	chunk_unload(v3i pos)
{
	if (MANAGER.chunks_count)
		MANAGER.chunks_count--;
}

void	chunks_render()
{
	for (int i = 0; i < MANAGER.chunks_count; i++)
		for (int y = 0; y < CHUNK_HEIGHT; y++)	
			for (int x = 0; x < CHUNK_WIDTH; x++)
				for (int z = 0; z < CHUNK_DEPTH; z++)
					blocks_manager_push(BLOCKS_MANAGER, 
						(block_desc){
							.type = MANAGER.chunks[i].blocks[INDEX(x, y, z)],
							.pos = v2_add(v2_from_v(MANAGER.chunks[i].pos), v2_of(x, y)),
							.z = MANAGER.chunks[i].pos.z + z,
						});

	blocks_manager_render(BLOCKS_MANAGER);
}

void	chunks_end()
{
	for (int i = 0; i < MANAGER.chunks_capacity; i++)
		free(MANAGER.chunks[i].blocks);
	free(MANAGER.chunks);
}
