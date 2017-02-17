#ifndef _MMU_H
#define _MMU_H

#include <stdlib.h>
#include <stdint.h>

#define PAGE_M 32 // 4G logical address space
#define PAGE_N 12 // 4K page size
#define ADDR_SPACE 4294967296 // (2 ^ PAGE_M)
#define PAGE_INDEX_SIZE (PAGE_M - PAGE_N) / 2 //10 bits (this is in BITS)
#define OFFSET_SIZE (PAGE_M - PAGE_INDEX_SIZE - PAGE_INDEX_SIZE)  // 20 bits (this is in BITS)
#define PAGE_SIZE 4096 //(2 ^ PAGE_N)
#define RPT_ENTRY_SIZE 2 // 2 bytes
#define RPT_SIZE (1024 * RPT_ENTRY_SIZE) // 2 ^ PAGE_INDEX_SIZE
#define PHYS_RAM_SIZE (RPT_SIZE + (PAGE_SIZE * 4))

#define SUCCESS         0x00
#define ILLEGAL_ADDRESS 0x01
#define ILLEGAL_ACCESS  0x02

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
uint8_t mmu_store(uint8_t *buf, uint32_t addr, size_t size);

/*
 * Fetches @size bytes from RAM into @buf
 *
 * @addr: the start of the data to fetch
 * @size: the number of bytes to fetch; may be larger or smaller than a page!
 */
uint8_t mmu_fetch(uint8_t *buf, uint32_t addr, size_t size);

/*
 * Fetches the root page table entry from RAM into @buf
 *
 * @max_size: the size of @buf to ensure the call does not overflow
 */
uint8_t mmu_fetch_rpt(uint8_t *buf, size_t max_size);

/*
 * Fetches both the root and user page table entries for @addr
 *
 * @max_size: the max number of bytes that the RPT and UPT entries may contain
 * @rpte: the fetched root page table entry
 * @upte: the fetched user page table entry
 */
uint8_t mmu_fetch_page_table_entries(uint8_t *rpte,
                                     uint8_t *upte,
                                     uint32_t addr,
                                     size_t max_size);

#endif
