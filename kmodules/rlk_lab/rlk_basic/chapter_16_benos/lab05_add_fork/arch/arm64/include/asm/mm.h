#ifndef	_MM_H
#define	_MM_H
#include <page.h>
#include <type.h>

#define TOTAL_MEMORY (SZ_1G)

#define LOW_MEMORY (2 * SECTION_SIZE)

#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long n);
#endif

#endif  /*_MM_H */
