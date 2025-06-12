#ifndef CHUNK_H
# define CHUNK_H

# include "block.h"
# include "math/aliases.h"

void	chunks_init(blocks_manager_t *bmanager);
void	chunk_load(v3i pos);
void	chunk_unload(v3i pos);
void	chunks_render();
void	chunks_end();

#endif