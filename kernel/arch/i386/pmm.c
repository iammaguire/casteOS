#include <string.h>
#include "kernel/multiboot.h"
#include "kernel/pmm.h"

uint32_t pmm_mem_size = 0;
uint32_t pmm_used_blocks = 0;
uint32_t pmm_max_blocks = 0;
uint32_t* pmm_mem_map = 0;

uint32_t* pmm_get_mem_bitmap()
{
	return pmm_mem_map;
}

void pmm_set_bitmap_address(uint32_t * addr)
{
	pmm_mem_map = addr;
}

void pmm_install(size_t mem_size, uint32_t* bitmap)
{
	pmm_mem_size = mem_size;
	pmm_mem_map = bitmap;
	pmm_max_blocks = (mem_size * 1024) / PMM_BLOCK_SIZE;
	pmm_used_blocks = pmm_get_block_count();

	uint32_t size = pmm_get_block_count() / PMM_BLOCKS_PER_BYTE;
	
	memset(pmm_mem_map, 0xff, size);
}

uint32_t get_mmap_end()
{
	return pmm_mem_map + pmm_mem_size;
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
	pmm_used_blocks--;
}

void* pmm_alloc_blocks(size_t size) {

	if (pmm_get_free_block_count() <= size)
		return 0;

	int frame = mmap_first_free_s(size);

	if (frame == -1)
		return 0;

	for (uint32_t i = 0;  i <size; i++)
		mmap_set(frame + i);

	uint32_t* addr = frame * PMM_BLOCK_SIZE;
	pmm_used_blocks += size;

	return (void*) addr;
}

void pmm_free_blocks(void* p, size_t size) {

	uint32_t* addr = (uint32_t*)  p;
	int frame = (uint32_t) addr / PMM_BLOCK_SIZE;

	for (uint32_t i = 0; i < size; i++)
		mmap_unset(frame + i);

	pmm_used_blocks -= size;
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