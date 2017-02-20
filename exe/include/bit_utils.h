#ifndef _BIT_UTILS_H
#define _BIT_UTILS_H

#include <assert.h>
#include <tberry/types.h>

u32 get_bit_mask(u32 len)
{
	return (1 << len) - 1;
}

/*
 * Extracts a subsection of @bits starting from @offset for @len bits.
 *
 * @start: the beginning of the sub bits.
 * @end: the end of the sub bits.
 */
u32 get_u32_sub_bits(u32 bits, u8 offset, u8 len)
{
	assert(0 <= offset && offset <= 31);
	assert(offset + len <= 32);

	u32 end = offset + len;
	u32 bit_mask = get_bit_mask(len);
	u32 shifted = bits >> (32 - end);
	return shifted & bit_mask;
}

/*
 * Extracts a subsection of @bits starting from @offset for @len bits.
 *
 * @start: the beginning of the sub bits.
 * @end: the end of the sub bits.
 */
u16 get_u16_sub_bits(u16 bits, u8 offset, u8 len)
{
	assert(0 <= offset && offset <= 15);
	assert(offset + len <= 16);

	u16 end = offset + len;
	u16 bit_mask = (u16) get_bit_mask(len);
	u16 shifted = bits >> (16 - end);
	return shifted & bit_mask;
}

/*
 * Stores the two bytes from @val into @buf
 */
void u16_to_bytes(u16 val, u8 *buf)
{
	buf[0] = (u8) get_u16_sub_bits(val, 0, 8);
	buf[1] = (u8) get_u16_sub_bits(val, 8, 8);
}

#endif
