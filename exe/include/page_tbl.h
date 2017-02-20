#ifndef _PAGE_TBL_H
#define _PAGE_TBL_H

#include <tberry/types.h>
#include "mmu.h"

struct page_tbl {
	u16 entries[TBL_SIZE];
};

#endif
