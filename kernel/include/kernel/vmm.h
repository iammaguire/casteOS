#ifndef HEADER_VMM_H
#define HEADER_VMM_H

#include <stdbool.h>
#include "kernel/pde.h"
#include "kernel/pte.h"

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024

#define PAGE_DIRECTORY_INDEX(x) 	(((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)			(((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PTABLE_ADDR_SPACE_SIZE 0x400000

#define DTABLE_ADDR_SPACE_SIZE 0x100000000

#define PAGE_SIZE 4096

struct ptable 
{
	pt_entry m_entries[PAGES_PER_TABLE];
};

typedef struct ptable ptable;

struct pdirectory {
	pd_entry m_entries[PAGES_PER_DIR];
};
typedef struct pdirectory pdirectory;


struct pageinfo {
	int pagetable;
	int page;
}__attribute((packed))__;
typedef struct pageinfo pageinfo;

extern void* vmm_alloc_page(pt_entry* e);
extern void vmm_free_page(pt_entry* e);
extern void vmm_initialize();
extern void vmm_map_page(void * phys, void * virt);

extern pt_entry* vmm_ptable_lookup_entry(ptable* p, uint32_t addr);
extern pd_entry * vmm_pdirectory_lookup_entry(pdirectory * p, uint32_t addr);

#endif