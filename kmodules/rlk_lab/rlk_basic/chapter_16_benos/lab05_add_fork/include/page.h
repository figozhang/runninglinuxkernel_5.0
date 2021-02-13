
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define PAGE_MASK (~(PAGE_SIZE-1))
/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1)&PAGE_MASK)

