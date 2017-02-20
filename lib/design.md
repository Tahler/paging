# Linux memory management

- Page size is 4kb
- Each page has unique page frame number

- A virtual address has two components:
  - Offset (12 bits (to index 4kb))
  - Virtual page frame number (rest of bits (4?))
- A page table entry has three components:
  - Valid flag
  - Physical page frame number
  - Access control info (writable?, executable?)
- Virtual address is translated by:
```
idx = virt_addr.vpfn
ppfn = page_table[idx]
base_addr = ppfn * PAGE_LEN
phys_addr = base_addr + virt_addr.offset
```
- On page fault, the OS pulls the page into an available physical page frame and
  adds an entry to the page table
- If there are no free physical pages, the system must make room
- The system puts pages into a swap file
- The oldest pages are the best candidates for swapping

- Linux has three levels of page tables
- The translation then must access page table 1, 2, and then 3 before finding a
  physical base address

## Data structures

- Physical pages are represented by the `mem_map` data structure, which is a
  list of `mem_map_t`
- Each `mem_map_t` describes a single page
- Important fields are:
  - `count`: the number of users of the page (shared memory)
  - `age`: a counter, incremented by swap daemon on each pass, reset by access
  - `map_nr`: the physical page frame number this `mem_map_t` represents

- Pages are allocated in blocks which are powers of 2 in size (i.e. blocks are 1
  page, 2 pages, 4 pages, 8 pages, etc.)
- The `free_area` vector is used to find and free pages
- Each element in the vector points to a `mem_map_t`
- `free_area` describes contiguously open blocks in powers of 2
- For example, element 2 of the array describes free and allocated blocks that
  are 4 pages long
- Allocation then tries to decompose blocks in the largest fashion
- Deallocation works the same way, combining free blocks if equal and adjacent
- Blocks may be as large as memory usage allows

# My design

## Constants

- `PFN_LEN = 10` bits
- `VPFN_0_OFFSET = 0` bits
- `VPFN_1_OFFSET = VPFN_0_OFFSET + PFN_LEN` bits
- `OFFSET_OFFSET = VPFN_1_OFFSET + PFN_LEN` bits
- `OFFSET_LEN = 12` bits
- `VIRT_ADDR_LEN = VPFN_0_OFFSET + VPFN_1_OFFSET + OFFSET_LEN` bits

- `ADDR_SPACE = 2^VIRT_ADDR_LEN` bytes

- `PAGE_LEN = 2^OFFSET_LEN` bytes

- `TBL_SIZE = 2^PFN_LEN` entries
- `TBL_ENTRY_LEN = 2` bytes
- `TBL_LEN = TBL_SIZE * TBL_ENTRY_LEN` bytes

- `PHYS_RAM_LEN = (4 * PAGE_LEN) + TBL_LEN` bytes

Note:
- LEN indicates length in either bits or bytes
- SIZE indicates length in elements

## Details

- Page size is 4kb
- Each page has a unique page frame number
- Two levels of page tables ("root" and "user")
- A virtual address has 3 components:
  - `rpn`: root page number
  - `upn`: user page number
  - `offset`: an offset from the base of the translated physical addres

- Each page table is represented by `page_tbl` which is an array of `tbl_entry`
- `tbl_entry` has two bit fields
  - Bit 0: Indicates if the page is currently in memory
    - If the page is on disk, there is no need to look at bits 1..16
  - Bit 1..16: Address to the next page table or physical address

- Physical pages are represented by `phys_pages` which is a list of `phys_page`
- A `phys_page` includes:
  - `ppfn`: the physical page frame number this page is at (the physical address
    can be deduced from this)
  - `age`: ??? should this be included?
- There are two swap files
  - User page table (2 KB) (indexed by `rpn * PAGE_LEN`)
  - Address space (4 GB) (indexed by `rpn * upn * PAGE_LEN`)

- Swapping will be put into a swap file
- Swapping will be determined by an LRU cache of pages
- The LRU cache will have to be a hashmap of int to a list node
- On access, the list node will have to be brought to the beginning
- If not in the cache, the page will have to be added to the cache and the tail
  will be purged and sent to disk

## TODO

[x] Get it to work without any swapping
[ ] Use/swap the first page only
[ ] Swap the address space
[ ] Swap the user page table
[ ] LRU cache of `phys_page`
