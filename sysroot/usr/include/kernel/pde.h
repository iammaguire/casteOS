#ifndef HEADER_PDE_H
#define HEADER_PDE_H

#include <stdint.h>

enum PAGE_PDE_FLAGS
{
	I86_PDE_PRESENT			=	0x1,
	I86_PDE_WRITABLE			=	0x2,
	I86_PDE_USER				=	0x4,
	I86_PDE_PWT				= 	0x8,
	I86_PDE_PCD				=	0x10,
	I86_PDE_ACCESSED			= 	0x20,
	I86_PDE_DIRTY				= 	0x40,
	I86_PDE_4MB				= 	0x80,
	I86_PDE_CPU_GLOBAL			= 	0x100,
	I86_PDE_LV4_GLOBAL			=	0x200,
	I86_PDE_FRAME			=	0x7FFFF000
};

typedef uint32_t pd_entry;

inline void pd_entry_add_attrib(pd_entry * e, uint32_t attrib);
inline void pd_entry_del_attrib(pd_entry * e, uint32_t attrib);
inline void pd_entry_set_frame(pd_entry * e, uint32_t addr);
inline int pd_entry_is_present(pd_entry e);
inline int pd_entry_is_user(pd_entry e);
inline int pd_entry_is_4mb(pd_entry e);
inline int pd_entry_is_writable(pd_entry e);
inline uint32_t pd_entry_pfn(pd_entry e);
inline void pd_entry_enable_global(pd_entry e);

#endif