#include <stdint.h>
#include "kernel/pde.h"

void pd_entry_add_attrib(pd_entry * e, uint32_t attrib)
{
	*e |= attrib;
}

void pd_entry_del_attrib(pd_entry * e, uint32_t attrib)
{
	*e &= ~attrib;
}

void pd_entry_set_frame(pd_entry * e, uint32_t addr)
{
	*e = (*e & ~I86_PDE_FRAME) | addr;
}

int pd_entry_is_present(pd_entry e)
{
	return e & I86_PDE_PRESENT;
}

int pd_entry_is_user(pd_entry e)
{
	return e & I86_PDE_USER;
}

int pd_entry_is_4mb(pd_entry e)
{
	return e & I86_PDE_PRESENT;
}

int pd_entry_is_writable(pd_entry e)
{
	return e & I86_PDE_WRITABLE;
}

uint32_t pd_entry_pfn(pd_entry e)
{
	return e & I86_PDE_FRAME;
}

void pd_entry_enable_global(pd_entry e)
{
	e = e;
}