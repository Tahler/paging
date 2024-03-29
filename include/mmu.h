#ifndef _MMU_H
#define _MMU_H

#include <tberry/types.h>

// Page frame number length in bits
#define PFN_LEN 10
// Offset length in bits
#define OFFSET_LEN 12
// 1st virtual page frame number bit position in virtual address
#define RPT_OFFSET 0
// 2nd virtual page frame number bit position in virtual address (10)
#define UPT_OFFSET (RPT_OFFSET + PFN_LEN)
// Offset bit position in virtual address (20)
#define OFFSET_OFFSET (UPT_OFFSET + PFN_LEN)

// Length of the virtual address in bits (32)
#define VIRT_ADDR_LEN (2 * PFN_LEN + OFFSET_LEN)
// Address space in bytes = 2^VIRT_ADDR_LEN (4 GB)
#define ADDR_SPACE (1 << VIRT_ADDR_LEN)
// Length of a page in bytes (4 KB)
#define PAGE_LEN (1 << OFFSET_LEN)
// Length of a page table entry in bytes
#define TBL_ENTRY_LEN 2
// Number of entries in a page table (1024)
#define PFN_RANGE (1 << PFN_LEN)

#define NUM_RPT_ENTRIES PFN_RANGE
// Length of the root page table in bytes (2048)
#define RPT_LEN (NUM_RPT_ENTRIES * TBL_ENTRY_LEN)

#define NUM_ENTRIES_PER_PAGE (PAGE_LEN / 2)
#define NUM_UPT_ENTRIES (NUM_RPT_ENTRIES * NUM_ENTRIES_PER_PAGE)
// Length of the user page table in bytes ()
#define UPT_LEN (NUM_UPT_ENTRIES * TBL_ENTRY_LEN)

#define NUM_PHYS_PAGES 4
// Length of physical memory in bytes (18432)
#define PHYS_RAM_LEN ((NUM_PHYS_PAGES * PAGE_LEN) + RPT_LEN)

// Return codes
#define SUCCESS		0x00
#define ILLEGAL_ADDRESS	0x01	// EFAULT
#define ILLEGAL_ACCESS	0x02	// EFAULT or EACCES

/*
 * Initializes the root page table (RPT).
 * Called on application startup.
 */
void mmu_init();

/*
 * Stores @buf in RAM starting at @addr
 *
 * @buf: the bytes to store in RAM
 * @addr: the start of the data
 * @size: the number of bytes in @buf, which may exceed a page size
 */
u8 mmu_store(u8 *buf, u32 addr, usize size);

/*
 * Fetches @size bytes from RAM into @buf
 *
 * @addr: the start of the data to fetch
 * @size: the number of bytes to fetch; may be larger or smaller than a page!
 */
u8 mmu_fetch(u8 *buf, u32 addr, usize size);

/*
 * Fetches the root page table entry from RAM into @buf
 *
 * @max_size: the size of @buf to ensure the call does not overflow
 */
u8 mmu_fetch_rpt(u8 *buf, usize max_size);

/*
 * Fetches both the root and user page table entries for @addr
 *
 * @max_size: the max number of bytes that the RPT and UPT entries may contain
 * @rpte: the fetched root page table entry
 * @upte: the fetched user page table entry
 */
u8 mmu_fetch_page_table_entries(u8 *rpte, u8 *upte, u32 addr, usize max_size);

#endif
