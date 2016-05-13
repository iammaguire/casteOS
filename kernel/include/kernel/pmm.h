#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include <stdint.h>
#include <stddef.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

extern uint32_t pmm_mem_size;
extern uint32_t pmm_used_blocks;
extern uint32_t pmm_max_blocks;
extern uint32_t* pmm_mem_map;

extern int pmm_first_free();
extern uint32_t pmm_get_block_size ();
extern uint32_t pmm_get_free_block_count ();
extern uint32_t pmm_get_use_block_count ();
extern uint32_t pmm_get_block_count ();
extern uint32_t* pmm_get_mem_bitmap();
extern uint32_t pmm_get_mem_size();
extern void pmm_init_region(uint32_t base, int size);
extern void pmm_deinit_region(uint32_t base, int size);
extern void* pmm_alloc_block();
extern void pmm_free_block(void* p);
extern void pmm_set_bitmap_address(uint32_t * addr);

inline void mmap_set(int bit)
{
	pmm_mem_map[bit / 32] |= (1  << (bit % 32));
}

inline void mmap_unset(int bit)
{
	pmm_mem_map[bit / 32] &= ~(1 << (bit % 32));
}

inline char mmap_test(int bit)
{
	return pmm_mem_map[bit / 32] & (1 << (bit % 32));
}

inline int mmap_first_free_s (size_t size) 
{

	if (size == 0)
		return -1;

	if (size == 1)
		return mmap_first_free();

	for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
		if (pmm_mem_map[i] != 0xffffffff)
		{
			for (int j = 0; j < 32; j++) 
			{
				int bit = 1 << j;

				if (!(pmm_mem_map[i] & bit) ) 
				{
					int starting_bit = i * 32;
					starting_bit += bit;

					uint32_t is_free = 0;

					for (uint32_t count = 0; count <= size; count++) {

						if (!mmap_test(starting_bit + count) )
							is_free++;	// this bit is clear (free frame)

						if (is_free == size)
							return i * 4 * 8 + j; //free count==size needed; return index
					}
				}
			}
		}

	return -1;
}

#endif