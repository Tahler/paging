#include <assert.h>
#include <mmu.h>

// TODO: create a parser for uint32_t -> a page address

/*
 * Initializes the root page table (RPT).
 * Called on application startup.
 */
void mmu_init()
{

}

/*
 * Stores @buf in RAM starting at @addr
 *
 * @buf: the bytes to store in RAM
 * @addr: the start of the data
 * @size: the number of bytes in @buf, which may exceed a page size
 */
uint8_t mmu_store(uint8_t *buf, uint32_t addr, size_t size)
{

}

/*
 * Fetches @size bytes from RAM into @buf
 *
 * @addr: the start of the data to fetch
 * @size: the number of bytes to fetch; may be larger or smaller than a page!
 */
uint8_t mmu_fetch(uint8_t *buf, uint32_t addr, size_t size)
{

}

/*
 * Fetches the root page table entry from RAM into @buf
 *
 * @max_size: the size of @buf to ensure the call does not overflow
 */
uint8_t mmu_fetch_rpt(uint8_t *buf, size_t max_size)
{

}

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
                                     size_t max_size)
{

}
