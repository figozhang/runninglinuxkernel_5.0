#ifndef _BENOS_TYPE_H
#define _LINUX_TYPE_H

#define SZ_1K				0x00000400
#define SZ_4K				0x00001000
#define SZ_1M				0x00100000
#define SZ_1G				0x40000000

#define NULL ((void *)0)

#ifdef __ASSEMBLY__
#define _AC(X,Y)	X
#define _AT(T,X)	X
#else
#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))
#endif

#define UL(x)		(_UL(x))
#define ULL(x)		(_ULL(x))

#define _UL(x)		(_AC(x, UL))
#define _ULL(x)		(_AC(x, ULL))

#define _BITUL(x)	(_UL(1) << (x))
#define _BITULL(x)	(_ULL(1) << (x))

#ifndef __ASSEMBLY__
typedef char s8;
typedef unsigned char u8;

typedef short s16;
typedef unsigned short u16;

typedef int s32;
typedef unsigned int u32;

typedef long long s64;
typedef unsigned long long u64;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#define offsetof(TYPE, MEMBER)	((long)&((TYPE *)0)->MEMBER)

#endif
#endif
