#ifndef INCLUDE_PAGE_UTIL_H
#define INCLUDE_PAGE_UTIL_H

#include "stdint.h"

extern int read_cr0();
extern int read_cr2();
extern int read_cr3();
extern int read_cr4();
extern void write_cr0(int value);
extern void write_cr3(int value);
extern void write_cr4(int value);
extern void invalidate_tlb(int page);
extern void enable_paging();

#endif
