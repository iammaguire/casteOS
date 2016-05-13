#include <stdint.h>
#include "kernel/pte.h"

void pt_entry_add_attrib(pt_entry * e, uint32_t attrib)
{
	*e |= attrib;
}

void pt_entry_del_attrib(pt_entry * e, uint32_t attrib)
{
	*e &= ~attrib;
}

void pt_entry_set_frame(pt_entry * e, uint32_t addr)
{
	*e = (*e & ~I86_PTE_FRAME) | addr;
}

uint32_t pt_entry_is_present(pt_entry e)
{
	return e & I86_PTE_PRESENT;
}

uint32_t pt_entry_is_writable(pt_entry e)
{
	return e & I86_PTE_FRAME;
}

uint32_t pt_entry_pfn(pt_entry e)
{
	return e & I86_PTE_FRAME;
}