#include "file.h"

#include "assert.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

char	*file_get(const char *path)
{
	FILE *f;
	size_t fsize;
	char *fdata;

	ASSERT(path, "no path given");
	f = fopen(path, "rb");
	ASSERT(f, "failed to open %s", path);

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	fdata = malloc(sizeof(char) * (fsize + 1));

	size_t byte_read = fread(fdata, 1, fsize, f);
	ASSERT(byte_read == fsize, 
		"Failed to read whole content of %s, file size: %zu but read %zu",
		path, fsize, byte_read);
	fdata[fsize] = 0;

	fclose(f);
	return fdata;
}
