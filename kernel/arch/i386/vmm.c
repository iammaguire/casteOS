#include <stdint.h>
#include <stdbool.h>
#include "kernel/vmm.h"
#include "kernel/pde.h"
#include "kernel/pde.h"
#include "kernel/page_util.h"

pdirectory * _cur_directory = 0;

uint32_t _cur_pdbr = 0;

inline pt_entry* vmm_ptable_lookup_entry(ptable* p, uint32_t addr) 
{

	if (p)
		return &p->m_entries[ PAGE_TABLE_INDEX (addr) ];
	return 0;
}

inline pd_entry* vmm_pdirectory_lookup_entry(pdirectory* p, uint32_t addr) 
{

	if (p)
		return &p->m_entries[ PAGE_TABLE_INDEX (addr) ];
	return 0;
}

inline bool vmm_switch_pdirectory(pdirectory* dir) 
{

	if (!dir)
		return false;

	_cur_directory = dir;
	paging_load_PDBR(_cur_pdbr);
	return true;
}

int vmm_commit_page(pt_entry * e)
{
	void *p = pmm_alloc_block();

	if (!p)
	{
		return 0;
	}

	pt_entry_set_frame(e, (uint32_t) p);
	pt_entry_add_attrib(e, I86_PTE_PRESENT);
	pt_entry_add_attrib(e, I86_PTE_WRITABLE);

	return 1;
}

void* vmm_alloc_page (pt_entry* e)
{
	void* p = pmm_alloc_block ();
	if (!p)
		return false;

	pt_entry_set_frame (e, (uint32_t) p);
	pt_entry_add_attrib (e, I86_PTE_PRESENT);
	
	return p;
}

void vmm_free_page(pt_entry* e) 
{
	void* p = (void*) pt_entry_pfn (*e);
	if (p)
		pmm_free_block (p);

	pt_entry_del_attrib (e, I86_PTE_PRESENT);
}

pdirectory * vmm_get_directory()
{
	return _cur_directory;
}

void vmm_flush_tld_entry(uint32_t addr)
{
	flush_TLB(addr);
}

void vmm_map_page(void * phys, void * virt)
{
	pdirectory * page_directory = vmm_get_directory();

	pd_entry * e = &page_directory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t) virt)];
	if (!pd_entry_is_present(*e))
	{
		ptable * new_table = (ptable *) pmm_alloc_block();
		if (!new_table)
		{
			return;
		}

		memset (new_table, 0, sizeof(ptable));

		pd_entry * entry = &page_directory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t) virt)];

		pd_entry_add_attrib (entry, I86_PDE_PRESENT);
      		pd_entry_add_attrib (entry, I86_PDE_WRITABLE);
      		pd_entry_set_frame (entry, (uint32_t) new_table);
	}

	ptable * table = (ptable *) PAGE_GET_PHYSICAL_ADDRESS(e);

	pt_entry * page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t) virt)];

	pt_entry_set_frame(page, (uint32_t) phys);
	pt_entry_add_attrib(page, I86_PTE_PRESENT);
	pt_entry_add_attrib(page, I86_PTE_WRITABLE);
}

void vmm_initialize()
{
	int i = 0;
	int frame = 0;
	int virt = 0;

	ptable * table = (ptable *) pmm_alloc_block();
	if (!table)
	{
		return;
	}

	ptable * table2 = (ptable *) pmm_alloc_block();
	if (!table2)
	{
		pmm_free_block(table);
		return;
	}	

	memset(table, 0, sizeof(ptable));

	for (i = 0, frame = 0x0, virt = 0x00000000; i < 1024; ++i, frame+=4096, virt+=4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, I86_PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		table2->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	for (i = 0, frame = 0x100000, virt = 0x100000; i < 1024; ++i, frame+=4096, virt+=4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, I86_PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		table->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	pdirectory * dir = (pdirectory *) pmm_alloc_blocks(3);

	if (!dir)
	{
		pmm_free_block(table);
		pmm_free_block(table2);
		return;
	}

	memset(dir, 0, sizeof(pdirectory));

	pd_entry * entry = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x100000)];
	pd_entry_add_attrib(entry, I86_PDE_PRESENT);
	pd_entry_add_attrib(entry, I86_PDE_WRITABLE);
	pd_entry_set_frame(entry, (uint32_t) table);

	pd_entry * entry2 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2, I86_PDE_PRESENT);
	pd_entry_add_attrib(entry2, I86_PDE_WRITABLE);
	pd_entry_set_frame(entry2, (uint32_t) table2);

	   _cur_pdbr = (uint32_t) &dir->m_entries;


	vmm_switch_pdirectory(dir);
	
	paging_enable(true);
}