#ifndef _VIRT_ADDR_H
#define _VIRT_ADDR_H

#include <tberry/types.h>
#include "bit_utils.h"

struct parsed_addr {
	u16 rpn;
	u16 upn;
	u16 offset;
};

struct parsed_addr parse_addr(u32 addr)
{
	struct parsed_addr parsed = {
		.rpn = get_u32_sub_bits(addr, 0, 10),
		.upn = get_u32_sub_bits(addr, 10, 10),
		.offset = get_u32_sub_bits(addr, 20, 12),
	};
	return parsed;
}

u32 get_virt_addr(u16 rpn, u16 upn, u16 offset)
{
	return ((rpn & get_bit_mask(10)) << 22)
		+ ((upn & get_bit_mask(10)) << 12)
		+ ((offset & get_bit_mask(12)) << 0);
}

#endif
