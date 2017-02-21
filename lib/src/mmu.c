#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tberry/types.h>

#include "mmu.h"
#include "fs.h"
#include "virt_addr.h"

#define RPT_OFFSET 0
#define PHYS_PAGES_OFFSET RPT_LEN
#define UPT_PAGE 0
#define UPT_PAGE_OFFSET (PHYS_PAGES_OFFSET + (UPT_PAGE * PAGE_LEN))
#define PHYS_PAGE 1
#define PHYS_PAGE_OFFSET (PHYS_PAGES_OFFSET + (PHYS_PAGE * PAGE_LEN))

static u8 ram[PHYS_RAM_LEN];
static u16 *rpt = (u16*) &ram[RPT_OFFSET];
// These are swapped into page 0 and page 1 respectively
static u16 *upt_page = (u16*) &ram[UPT_PAGE_OFFSET];
static u16 *phys_page = (u16*) &ram[PHYS_PAGE_OFFSET];

void mmu_init()
{

}

/*
 * Returns true if the highest order bit is set in @entry, indicating that the
 * lowest 12 bits are an address in me
 */
bool next_is_in_memory(u16 entry)
{
	return (entry >> 15) & 0b1;
}

usize get_swap_addr(u16 rpn, u16 upn, u16 offset)
{
	return rpn * PAGE_LEN * PAGE_LEN
		+ upn * PAGE_LEN
		+ offset;
}

/*
 * Finds (or creates) the base physical address to access the user page table
 * entry for @rpn.
 *
 * Note: the UPT page is always loaded at UPT_PAGE_OFFSET.
 */
usize get_upt_base_addr(u16 rpn)
{
	if (!next_is_in_memory(rpt[rpn])) {
		// TODO: save the old page
		usize swap_addr = get_swap_addr(rpn, 0, 0);
		fs_load_buf("swapfile", swap_addr, upt_page, PAGE_LEN);
		u16 loaded_addr = UPT_PAGE;
		// Set in mem bit
		u16 new_entry = loaded_addr | 0x8000;
		rpt[rpn] = new_entry;
	}
	assert((rpt[rpn] & 0xFFF) == UPT_PAGE);
	return UPT_PAGE;
}

/*
 * Finds (or creates) the base physical address to access the physical page for
 * @upn.
 *
 * Note: the base address of the UPT is not needed because this implementation
 * always has the current page of the UPT at address UPT_PAGE_OFFSET.
 */
usize get_phys_base_addr(u16 rpn, u16 upn)
{
	if (!next_is_in_memory(upt_page[upn])) {
		usize swap_addr = get_swap_addr(rpn, upn, 0);
		fs_load_buf("swapfile", swap_addr, phys_page, PAGE_LEN);

		u16 loaded_addr = PHYS_PAGE;
		u16 new_entry = loaded_addr | 0x8000;
		upt_page[upn] = new_entry;
	}
	assert((upt_page[upn] & 0xFFF) == PHYS_PAGE);
	return PHYS_PAGE;
}

/*
 * Finds (or creates) an address in RAM to access the virtual address.
 * Note: does not handle page overflow.
 */
usize get_phys_addr(u16 rpn, u16 upn, u16 offset)
{
	/*usize upt_base_addr =*/ get_upt_base_addr(rpn);
	usize phys_base_addr = get_phys_base_addr(rpn, upn);
	usize phys_addr = phys_base_addr + offset;
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

		usize phys_addr = get_phys_addr(va.rpn, va.upn, va.offset);
		memcpy(&ram[phys_addr], &buf[buf_pos], copy_len);
		num_remaining_bytes -= copy_len;
		buf_pos += copy_len;

		if (overflow) {
			bool up_overflow = (va.upn + 1) >= RPT_SIZE;
			if (up_overflow) {
				bool rp_overflow = (va.rpn + 1) >= RPT_SIZE;
				if (rp_overflow) {
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

		usize phys_addr = get_phys_addr(va.rpn, va.upn, va.offset);
		memcpy(&buf[buf_pos], &ram[phys_addr], copy_len);
		num_remaining_bytes -= copy_len;
		buf_pos += copy_len;

		if (overflow) {
			bool up_overflow = (va.upn + 1) >= RPT_SIZE;
			if (up_overflow) {
				bool rp_overflow = (va.rpn + 1) >= RPT_SIZE;
				if (rp_overflow) {
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

// u8 mmu_fetch_rpt(u8 *buf, usize max_size)
// {
// 	// u16 rpt_entry = rpt[parsed]
// }

// u8 mmu_fetch_page_table_entries(u8 *rpte, u8 *upte, u32 addr, usize max_size)
// {
// 	struct parsed_addr parsed = parse_addr(addr);

// 	u16 rpt_entry = rpt[parsed.rpn];
// 	u16_to_bytes(rpt_entry, rpte);

// 	u16 upt_entry = upt[rpt_entry];
// 	u16_to_bytes(upt_entry, upte);
// }

// TODO: should be a single swap file
/*
 * Saves the user page to disk, stored at fs/pages/@rpt_idx/@upt_idx
 */
// u8 save_page(u16 rpt_idx, u16 upt_idx, u8 *page_buf)
// {
// 	// strlen("fs/65535/65535") == 14
// 	char file_name[14];
// 	sprintf(file_name, "fs/%d/%d", rpt_idx, upt_idx)
// 	return write_buf(file_name, page_buf, PAGE_LEN);
// }
