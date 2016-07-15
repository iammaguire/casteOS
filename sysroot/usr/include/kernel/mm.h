#define MEMORY_AVAILABLE		1
#define MEMORY_RESERVED			2
#define MEMORY_ACPI_TABLES	3
#define MEMORY_ACPI_NVS			4
#define MEMORY_BAD			    5

#define PAGE_PRESENT			  1
#define PAGE_RW				      2
#define PAGE_USER			      4
#define PAGE_WRITE_THROUGH	8

#define PAGE_TABLE			    0xF01000
#define PAGE_DIRECTORY			0xF00000

#define PMM_BITMAP			    0x1400000

#define REQUIRED_MEMORY			64

#define KERNEL_HEAP			    0xCF000000
#define USER_HEAP			      0xD8000000

typedef struct mmap
{
	unsigned int entry_size;
	uint64_t base;
	uint64_t size;
	unsigned int type;
} __attribute__((packed)) mmap_t;

uint64_t total_memory;
uint64_t usable_memory;

uint32_t total_memory_mb;
uint32_t usable_memory_mb;
uint32_t reserved_memory_mb;

uint32_t total_memory_pages;
uint32_t usable_memory_pages;
uint32_t reserved_memory_pages;

uint32_t used_memory_pages;
uint32_t free_memory_pages;

extern void mm_init();
extern void mm_parse_map();
extern void mm_print_mem_type(int type);
extern void mm_init_paging();
extern void mm_map_page(void* virtual, void* physical, int count, char flags);
extern void mm_unmap_page(void* virtual, int count);
extern void mm_init_pmm();
extern void pmm_mark_page_used(void* page);
extern void pmm_mark_page_free(void* page);
extern void pmm_mark_used(void* base, int count);
extern void pmm_mark_free(void* base, int count);
extern bool vmm_is_page_free(void* address);
extern void* vmm_get_free_page(void* start, int count);
extern void* pmm_alloc(int pages);
extern void* vmm_alloc(void* start, int pages, char flags);
extern void* vmm_get_free_page(void* start, int count);
extern void vmm_free(void* address, int pages);
extern int bytes_to_pages(int bytes);
extern void* kmalloc(int size);
extern void kfree(void* address);
extern void* malloc(int size);
extern void free(void* address);
