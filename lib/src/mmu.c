#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tberry/types.h>
#include <array_utils.h>

#include "mmu.h"
#include "fs.h"
#include "virt_addr.h"

#define RPT_OFFSET 0
#define PHYS_PAGES_OFFSET RPT_LEN
#define UPT_PAGE 0
#define UPT_PAGE_OFFSET (PHYS_PAGES_OFFSET + (UPT_PAGE * PAGE_LEN))
#define PHYS_PAGE 1
#define PHYS_PAGE_OFFSET (PHYS_PAGES_OFFSET + (PHYS_PAGE * PAGE_LEN))
#define VAR_PAGE 2
#define VAR_PAGE_OFFSET (PHYS_PAGES_OFFSET + (VAR_PAGE * PAGE_LEN))
#define LOADED_RPN_OFFSET (VAR_PAGE_OFFSET + 0)
#define LOADED_UPN_OFFSET (VAR_PAGE_OFFSET + 2)

static char *swapfile = "swapfile";

u8 ram[PHYS_RAM_LEN];
u16 *rpt = (u16 *) (ram + RPT_OFFSET);
// These are swapped into page 0 and page 1 respectively
u16 *upt_page = (u16 *) (ram + UPT_PAGE_OFFSET);
u8 *phys_page = (u8 *) (ram + PHYS_PAGE_OFFSET);
// Intended to be used for one element: the current loaded virtual address
u16 *loaded_rpn = (u16 *) (ram + LOADED_RPN_OFFSET);
u16 *loaded_upn = (u16 *) (ram + LOADED_UPN_OFFSET);

void mmu_init()
{
	fs_delete_file(swapfile);
}

/*
 * Sets the first bit on each entry in @buf for @len entries
 */
void set_flags(u16 *buf, usize len)
{
	for (int i = 0; i < len; i++) {
		buf[i] |= 0x8000;
	}
}

/*
 * Clears the first bit on each entry in @buf for @len entries
 */
void clear_flags(u16 *buf, usize len)
{
	for (int i = 0; i < len; i++) {
		buf[i] &= ~0x8000;
	}
}

/*
 * Returns true if the highest order bit is set in @entry, indicating that the
 * lowest 12 bits are an address in me
 */
bool next_is_in_memory(u16 entry)
{
	return (entry >> 15) & 0b1;
}

/*
 * Returns the absolute address of the page of the user page table in the
 * swapfile
 */
usize get_upt_swap_addr(u16 rpn)
{
	return rpn * PAGE_LEN;
}

/*
 * Returns the absolute address of the physical page in the swapfile
 */
usize get_phys_swap_addr(u16 rpn, u16 upn)
{
	// Physical pages in the swap file start immediately after the UPT
	usize phys_pages_offset = UPT_LEN;

	usize upt_base_addr = rpn * PFN_RANGE * PAGE_LEN;
	usize phys_base_addr = upt_base_addr + (upn * PAGE_LEN);
	return phys_pages_offset + phys_base_addr;
}

/*
 * Loads the UPT page at @rpn from the swapfile into the upt_page buffer
 */
u8 load_upt_page(u16 rpn)
{
	// UPT is not in memory anymore
	clear_flags(&rpt[*loaded_rpn], 1);
	clear_flags(&upt_page[*loaded_upn], 1);
	usize swap_addr = get_upt_swap_addr(rpn);
	u8 return_code =
	    fs_load_buf(swapfile, swap_addr, (u8 *) upt_page, PAGE_LEN);
	rpt[rpn] = UPT_PAGE;
	set_flags(&rpt[rpn], 1);
	*loaded_rpn = rpn;
	return return_code;
}

/*
 * Saves the currently loaded UPT page into the swapfile, clearing the
 * "in-memory" flag on the way
 */
u8 save_upt_page()
{
	usize swap_addr = get_upt_swap_addr(*loaded_rpn);
	clear_flags(upt_page, PAGE_LEN);
	return fs_save_buf(swapfile, swap_addr, (u8 *) upt_page, PAGE_LEN);
}

/*
 * Loads the physical page at @rpn, @upn from the swapfile into the phys_page
 * buffer
 */
u8 load_phys_page(u16 rpn, u16 upn)
{
	usize swap_addr = get_phys_swap_addr(rpn, upn);
	u8 return_code = fs_load_buf(swapfile, swap_addr, phys_page, PAGE_LEN);
	upt_page[upn] = PHYS_PAGE;
	set_flags(&upt_page[upn], 1);
	*loaded_upn = upn;
	return return_code;
}

u8 save_phys_page()
{
	usize swap_addr = get_phys_swap_addr(*loaded_rpn, *loaded_upn);
	return fs_save_buf(swapfile, swap_addr, phys_page, PAGE_LEN);
}

void swap(u16 rpn, u16 upn)
{
	save_upt_page();
	save_phys_page();

	load_upt_page(rpn);
	load_phys_page(rpn, upn);
}

/*
 * Loads the correct pages into memory and returns the physical address into the
 * loaded physical page.
 *
 * Note: does not handle page overflow.
 */
u16 get_ppn(u16 rpn, u16 upn, u16 offset)
{
	swap(rpn, upn);
	return PHYS_PAGE + offset;
}

usize get_phys_addr(u16 ppn)
{
	return PHYS_PAGES_OFFSET + (ppn * PAGE_LEN);
}

u8 cpy_loop(u32 addr, u8 *src, u8 *dest, usize size, void cpy_fn(u8 *, u8 *, usize, usize, usize))
{
	struct parsed_addr va = parse_addr(addr);
	usize num_remaining_bytes = size;
	usize buf_pos = 0;
	do {
		bool overflow = (va.offset + num_remaining_bytes) > PAGE_LEN;
		usize copy_len = overflow
		    ? PAGE_LEN - va.offset : num_remaining_bytes;

		u16 ppn = get_ppn(va.rpn, va.upn, va.offset);
		usize phys_addr = get_phys_addr(ppn);
		cpy_fn(dest, src, buf_pos, phys_addr, copy_len);
		num_remaining_bytes -= copy_len;
		buf_pos += copy_len;

		if (overflow) {
			bool up_overflow = (va.upn + 1) >= NUM_RPT_ENTRIES;
			if (up_overflow) {
				bool rp_overflow =
				    (va.rpn + 1) >= NUM_RPT_ENTRIES;
				if (rp_overflow) {
					// TODO: do this ahead of time
					// Error
					return ILLEGAL_ADDRESS;
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

void store_mem_cpy(u8 *buf, u8 *ram, usize buf_offset, usize ram_offset, usize size)
{
	memcpy(&ram[ram_offset], &buf[buf_offset], size);
}

void fetch_mem_cpy(u8 *ram, u8 *buf, usize buf_offset, usize ram_offset, usize size)
{
	memcpy(&buf[buf_offset], &ram[ram_offset], size);
}

u8 mmu_store(u8 *buf, u32 addr, usize size)
{
	return cpy_loop(addr, ram, buf, size, *store_mem_cpy);
}

u8 mmu_fetch(u8 *buf, u32 addr, usize size)
{
	return cpy_loop(addr, buf, ram, size, *fetch_mem_cpy);
}

/*
 * Stores the entire RPT into @buf
 *
 * Note: ignores @max_size
 */
u8 mmu_fetch_rpt(u8 *buf, usize max_size)
{
	memcpy(buf, (u8 *)rpt, RPT_LEN);
	return SUCCESS;
}

u8 mmu_fetch_page_table_entries(u8 *rpte, u8 *upte, u32 addr, usize max_size)
{
	struct parsed_addr pa = parse_addr(addr);
	swap(pa.rpn, pa.upn);
	memcpy(rpte, (u8 *)(&rpt[pa.rpn]), 2);
	memcpy(upte, (u8 *)(&upt_page[pa.upn]), 2);
	return SUCCESS;
}
