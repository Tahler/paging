#ifndef _FS_H
#define _FS_H

#include <stdio.h>
#include <errno.h>
#include <tberry/types.h>

/*
 * Overwrites the file at @path to contain @buf
 */
u8 write_buf(char *path, u8 *buf, usize size)
{
	// Open file for writing in binary
	FILE *file = fopen(file_name, "wb");
	// `1` indicates the nobj
	usize ok = fwrite(buf, size, 1, file);
	fclose(file);
	return ok ? 0 : 1;
}

u8 read_buf(char *file_name, u8 *buf)
{
	// Open file for writing in binary
	FILE *file = fopen(file_name, "wb");
	// `1` indicates the nobj, which is 1 buffer
	usize ok = fread(buf, size, 1, file);
	fclose(file);
	return ok ? 0 : 1;
}

#endif
