#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tberry/types.h>

#include "mmu.h"
#include "fs.h"
#include "virt_addr.h"

struct root_page_tbl {
	u16 entries[RPT_SIZE];
};

struct user_page_tbl {
	u16 entries[UPT_SIZE];
}

// #define USABLE_RAM_LEN = (PHYS_RAM_LEN - RPT_LEN)
// static u8 ram[USABLE_RAM_LEN];

// Rpt is always available
static struct root_page_tbl rpt;
// TODO: these should be paged
static struct user_page_tbl upt;
static u8 phys[ADDR_SPACE];

void mmu_init()
{

}

/*
 * Finds (or creates) an address in RAM to access the virtual address.
 * Note: does not handle page overflow.
 */
usize get_phys_addr(struct parsed_addr parsed)
{
	u16 rpte = rpt.entries[parsed.rpn];
	// TODO: instead index ram starting at the loaded page base addr
	u16 upte = upt.entries[parsed.upn];
	usize base_addr = ((usize) upte) * PAGE_LEN;
	usize phys_addr = base_addr + parsed.offset;
	return phys_addr;
}

u8 mmu_store(u8 *buf, u32 addr, usize size)
{
	struct parsed_addr va = parse_addr(addr);
	usize num_remaining_bytes = size;
	usize buf_pos = 0;
	do {
		bool overflow = (va.offset + num_remaining_bytes) > PAGE_LEN;
		usize copy_len = overflow
			? PAGE_LEN - va.offset
			: num_remaining_bytes;

		usize phys_addr = get_phys_addr(va);
		memcpy(&ram[phys_addr], &buf[buf_pos], copy_len);
		num_remaining_bytes -= copy_len;
		buf_pos += copy_len;

		if (overflow) {
			bool up_overflow = (va.upn + 1) >= RPT_SIZE;
			if (up_overflow) {
				bool rp_overflow = (va.rpn + 1) >= RPT_SIZE;
				if (overflows_rp) {
					// TODO: resolve
					// Error
					printf("ERROR - overflowed rpt");
					exit(1);
				} else {
					va.rpn += 1;
				}
				va.upn = 0;
			} else {
				va.upn += 1;
			}
			va.offset = 0;
		}
	} while (num_remaining_bytes > 0);
	return SUCCESS;
}

u8 mmu_fetch(u8 *buf, u32 addr, usize size)
{
	struct parsed_addr va = parse_addr(addr);
	usize num_remaining_bytes = size;
	usize buf_pos = 0;
	do {
		bool overflow = (va.offset + num_remaining_bytes) > PAGE_LEN;
		usize copy_len = overflow
			? PAGE_LEN - va.offset
			: num_remaining_bytes;

		usize phys_addr = get_phys_addr(va);
		memcpy(&buf[buf_pos], &ram[phys_addr], copy_len);
		num_remaining_bytes -= copy_len;
		buf_pos += copy_len;

		if (overflow) {
			bool up_overflow = (va.upn + 1) >= RPT_SIZE;
			if (up_overflow) {
				bool rp_overflow = (va.rpn + 1) >= RPT_SIZE;
				if (overflows_rp) {
					// TODO: resolve
					// Error
					printf("ERROR - overflowed rpt");
					exit(1);
				} else {
					va.rpn += 1;
				}
				va.upn = 0;
			} else {
				va.upn += 1;
			}
			va.offset = 0;
		}
	} while (num_remaining_bytes > 0);
	return SUCCESS;
}

u8 mmu_fetch_rpt(u8 *buf, usize max_size)
{
	// u16 rpt_entry = rpt[parsed]
}

u8 mmu_fetch_page_table_entries(u8 *rpte, u8 *upte, u32 addr, usize max_size)
{
	struct parsed_addr parsed = parse_addr(addr);

	u16 rpt_entry = rpt[parsed.rpn];
	u16_to_bytes(rpt_entry, rpte);

	u16 upt_entry = upt[rpt_entry];
	u16_to_bytes(upt_entry, upte);
}

// TODO: should be a single swap file
/*
 * Saves the user page to disk, stored at fs/pages/@rpt_idx/@upt_idx
 */
u8 save_page(u16 rpt_idx, u16 upt_idx, u8 *page_buf)
{
	// strlen("fs/65535/65535") == 14
	char file_name[14];
	sprintf(file_name, "fs/%d/%d", rpt_idx, upt_idx)
	return write_buf(file_name, page_buf, PAGE_LEN);
}
