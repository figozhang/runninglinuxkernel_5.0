#ifndef __COMPILER_H_INCLUDE__
#define __COMPILER_H_INCLUDE__

/* compiler specific */

#ifdef __GNUC__
#include <target/compiler_gcc.h>
#endif

#ifndef __always_inline
#define __always_inline	inline
#endif

#endif /* __COMPILER_H_INCLUDE__ */
