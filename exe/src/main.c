#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <tberry/types.h>

#include "mmu.h"
/*
 * Returns true if the bits across both arrays are equal.
 */
bool arrays_are_equal(const void *a1, const void *a2, usize len)
{
	return memcmp(a1, a2, len) == 0;
}

void test_can_store_and_fetch()
{
	u8 store[6] = { 1, 2, 3, 4, 5, 6 };
	u32 rp0_up0 = 0x00000000;
	mmu_store(store, rp0_up0, 6);
	u32 rp1_up1 = 0x00401000;
	mmu_store(store, rp1_up1, 6);

	u8 load[6];
	mmu_fetch(load, rp0_up0, 6);
	assert(arrays_are_equal(store, load, 6));
	mmu_fetch(load, rp1_up1, 6);
	assert(arrays_are_equal(store, load, 6));
	mmu_fetch(load, rp1_up1, 6);
	assert(arrays_are_equal(store, load, 6));
}

void test_can_store_and_fetch_overflow()
{
	u8 store[4097];
	store[0] = 24;
	store[4096] = 42;
	u32 rp0_up0 = 0x00000000;
	mmu_store(store, rp0_up0, 4097);

	u8 load_all[4097];
	mmu_fetch(load_all, rp0_up0, 4097);
	assert(load_all[0] == 24);
	assert(load_all[4096] == 42);

	u32 rp0_up1 = 0x00001000;
	u8 load_end[1];
	mmu_fetch(load_end, rp0_up1, 1);
	assert(load_end[0] == 42);
	assert(arrays_are_equal(&store[4096], load_end, 1));
}

int main()
{
	test_can_store_and_fetch();
	// test_can_store_and_fetch_overflow();
}
