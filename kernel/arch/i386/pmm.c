#include "kernel/multiboot.h"
#include "kernel/pmm.h"
#include <string.h>

void pmm_install(struct multiboot* mbd, uint32_t mem_size, uint32_t* mem_map)
{
	printf("\tPMM initialized with %s MB of usable memory\n", itoa_nbuf(mem_size / 1024, 10));
	printf("\tLower memory: %s KB\n", itoa_nbuf(mbd->mem_lower, 10));
	printf("\tUpper memory: %s MB\n", itoa_nbuf(mbd->mem_upper / 1024, 10));
	printf("\tFlags: 0b%s\n", itoa_nbuf(mbd->flags, 2));
	printf("\tMemory map length: 0x%s\n", itoa_nbuf(mbd->mmap_length, 16));
	printf("\tMemory map address: 0x%s\n", itoa_nbuf(mbd->mmap_addr, 16));
	
	pmm_mem_size = mem_size;
	pmm_mem_map = (uint32_t*) mem_map;
	pmm_max_blocks = (pmm_get_mem_size() * 1024) / PMM_BLOCK_SIZE;
	pmm_used_blocks = pmm_max_blocks;

	memset(pmm_mem_map, 0xf, pmm_get_block_count() / PMM_BLOCKS_PER_BYTE);
}

int mmap_first_free()
{
	for(uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
	{
		if(pmm_mem_map[i] != 0xffffffff)
		{
			for(int j = 0; j < 32; j++)
			{
				int bit = 1 << j;
				if(!(pmm_mem_map[i] & bit))
					return i * 4 * 8 + j;
			}
		}
	}

	return -1;
}

void pmm_init_region(uint32_t base, int size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for(;blocks > 0; blocks--)
	{
		mmap_unset(align++);
		pmm_used_blocks--;
	}

	mmap_set(0);
}

void pmm_deinit_region(uint32_t base, int size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for(;blocks > 0; blocks--)
	{
		mmap_set(align++);
		pmm_used_blocks++;
	}

	mmap_set(0);
}

void* pmm_alloc_block()
{
	if(pmm_get_free_block_count() <= 0)
		return 0;

	int frame = mmap_first_free();

	if(frame == -1)
		return 0;

	mmap_set(frame);

	uint32_t* addr = frame * PMM_BLOCK_SIZE;
	pmm_used_blocks++;

	return (void*) addr;
}

void pmm_free_block(void* p)
{
	uint32_t addr = (uint32_t) p;
	int frame = addr / PMM_BLOCK_SIZE;

	mmap_unset(frame);
	pmm_used_blocks--;;
}

uint32_t pmm_get_mem_size()
{
	return pmm_mem_size;
}

uint32_t pmm_get_block_count () 
{
	return pmm_max_blocks;
}

uint32_t pmm_get_use_block_count () 
{
	return pmm_used_blocks;
}

uint32_t pmm_get_free_block_count () 
{
	return pmm_max_blocks - pmm_used_blocks;
}

uint32_t pmm_get_block_size () 
{
	return PMM_BLOCK_SIZE;
}