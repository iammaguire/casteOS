#include <stdint.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCK_ALIGN PMM_BLOCK_SIZE

char* strMemoryTypes[] = {
	{"Available"},			//memory_region.type==0
	{"Reserved"},			//memory_region.type==1
	{"ACPI Reclaim"},		//memory_region.type==2
	{"ACPI NVS Memory"}		//memory_region.type==3
};

uint32_t pmm_mem_size = 0;
uint32_t pmm_used_blocks = 0;
uint32_t pmm_max_blocks = 0;
uint32_t* pmm_mem_map = 0;

extern int pmm_first_free();
extern uint32_t pmm_get_block_size ();
extern uint32_t pmm_get_free_block_count ();
extern uint32_t pmm_get_use_block_count ();
extern uint32_t pmm_get_block_count ();
extern uint32_t pmm_get_mem_size();
extern void pmm_init_region(uint32_t base, int size);
extern void pmm_deinit_region(uint32_t base, int size);
extern void* pmm_alloc_block();
extern void pmm_free_block(void* p);

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