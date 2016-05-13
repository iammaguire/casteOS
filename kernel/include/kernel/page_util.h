#ifndef INCLUDE_PAGE_UTIL_H
#define INCLUDE_PAGE_UTIL_H

#include "stdint.h"

void paging_load_PDBR (uint32_t addr);
uint32_t paging_get_PDBR();
void flush_TLB(uint32_t addr);
void paging_enable(int enable);

#endif