#ifndef _FS_H
#define _FS_H

#include <stdio.h>
#include <errno.h>

#include <tberry/types.h>
#include <array_utils.h>

u8 fs_delete_file(char *path)
{
	return remove(path);
}

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
u8 fs_load_buf(char *path, usize src_addr, u8 *dest, usize len)
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
			printf("before\n");
			peek_bytes(dest, 10);
			printf("loading from %lu in file\n", src_addr);
			printf("result: ");
			ok = fread(dest, len, 1, file);
			if (!ok) {
				printf("read failed, filling with 0s\n");
				for (int i = 0; i < len; i++) {
					dest[i] = 0;
				}
			}
			peek_bytes(dest, 10);
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

/*
 * Reads a buffer from the file at @path starting at @src_addr, into @dest for
 * @len bytes.
 */
u8 fs_save_buf(char *path, usize dest_addr, u8 *src, usize len)
{
	fs_create_file_if_not_exists(path);
	// Open file for writing in binary
	FILE *file = fopen(path, "r+b");
	bool ok = false;
	if (file) {
		ok = !fseek(file, dest_addr, SEEK_SET);
		if (ok) {
			// `1` indicates the nobj
			// If less than 1 is returned (i.e. 0), then `ok` is
			// false (i.e. 0)
			printf("saving");
			peek_bytes(src, 10);
			printf("to %lu in file\n", dest_addr);
			ok = fwrite(src, len, 1, file);
			if (!ok) {
				printf("save failed.\n");
			}
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

#endif
