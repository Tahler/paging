#ifndef _FS_H
#define _FS_H

#include <stdio.h>
#include <errno.h>
#include <tberry/types.h>

u8 fs_create_file_if_not_exists(char *path)
{
	FILE *file = fopen(path, "a");
	if (file) {
		fclose(file);
	}
	return errno;
}

/*
 * Reads a buffer from the file at @path starting at @src_addr, into @dest for
 * @len bytes.
 */
u8 fs_load_buf(char *path, usize src_addr, void *dest, usize len)
{
	fs_create_file_if_not_exists(path);
	// Open file for writing in binary
	FILE *file = fopen(path, "rb");
	bool ok = false;
	if (file) {
		ok = !fseek(file, src_addr, SEEK_SET);
		if (ok) {
			// `1` indicates the nobj
			// If less than 1 is returned (i.e. 0), then `ok` is
			// false (i.e. 0)
			ok = fread(dest, len, 1, file);
		} else {
			printf("Error: %s\n", strerror(errno));
			printf("Error: %s\n", strerror(ferror(file)));
		}
		fclose(file);
	} else {
		printf("Error: %s\n", strerror(errno));
	}
	return ok ? 0 : 1;
}

// /*
//  * Overwrites the file at @path to contain @buf
//  */
// u8 write_buf(char *path, u8 *buf, usize size)
// {
// 	// Open file for writing in binary
// 	FILE *file = fopen(path, "wb");
// 	// `1` indicates the nobj
// 	usize ok = fwrite(buf, size, 1, file);
// 	fclose(file);
// 	return ok ? 0 : 1;
// }

// u8 read_buf(char *path, u8 *buf, usize size)
// {
// 	// Open file for writing in binary
// 	FILE *file = fopen(path, "wb");
// 	// `1` indicates the nobj, which is 1 buffer
// 	usize ok = fread(buf, size, 1, file);
// 	fclose(file);
// 	return ok ? 0 : 1;
// }

#endif
