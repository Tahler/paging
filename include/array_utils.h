#ifndef _ARRAY_UTILS_H
#define _ARRAY_UTILS_H

#include <stdio.h>
#include <tberry/types.h>

void peek_bytes(u8 *a, usize len)
{
	printf("[");
	for (int i = 0; i < len - 1; i++) {
		printf("%d, ", a[i]);
	}
	printf("%d]\n", a[len - 1]);
}

#endif
