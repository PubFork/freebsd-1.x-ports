/* $Id: segmem.h,v 1.1 1994/02/24 08:05:35 hsu Exp $
 */
/*
 * Copyright  Robert J. Amstadt, 1993
 */
#ifndef SEGMEM_H
#define SEGMEM_H

#ifdef __linux__
#define HAVE_IPC
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__)
#define HAVE_IPC
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMSEG 32     /* XXX SEMMNI /usr/src/sys/conf/param.h */
#define SHM_RANGE_START       0x40000000
#endif

/*
 * Array to track selector allocation.
 */
#define MAX_SELECTORS		512
#define SELECTOR_ISFREE		0x8000
#define SELECTOR_INDEXMASK	0x0fff

extern unsigned short SelectorMap[MAX_SELECTORS];

#ifdef HAVE_IPC
#define SAFEMAKEPTR(s, o) (((int) (s) << 16) | ((o) & 0xffff))
#define FIXPTR(p)	  (p)
#else
#define SAFEMAKEPTR(s, o) \
    (((int) SelectorMap[SelectorMap[(s) >> 3] & SELECTOR_INDEXMASK] << 19) \
     | 0x70000 | ((o) & 0xffff))
#define FIXPTR(p)	  SAFEMAKEPTR((unsigned long) (p) >> 16, (p))
#endif

/*
 * Structure to hold info about each selector we create.
 */

typedef struct segment_descriptor_s
{
    void          *base_addr;	/* Pointer to segment in flat memory	*/
    unsigned int   length;	/* Length of segment			*/
    unsigned int   flags;	/* Segment flags (see neexe.h and below)*/
    unsigned short selector;	/* Selector used to access this segment */
    unsigned short owner;	/* Handle of owner program		*/
    unsigned char  type;	/* DATA or CODE				*/
#ifdef HAVE_IPC
    key_t	   shm_key;	/* Shared memory key or IPC_PRIVATE     */
#endif
} SEGDESC;

extern int IPCCopySelector(int i_old, unsigned long new, int swap_type);

/*
 * Additional flags
 */
#define NE_SEGFLAGS_MALLOCED	0x00010000 /* Memory allocated with malloc() */

/*
 * Global memory flags
 */
#define GLOBAL_FLAGS_MOVEABLE		0x0002
#define GLOBAL_FLAGS_ZEROINIT		0x0040
#define GLOBAL_FLAGS_CODE		0x00010000
#define GLOBAL_FLAGS_EXECUTEONLY	0x00020000
#define GLOBAL_FLAGS_READONLY		0x00020000

#define FIRST_SELECTOR	8

static __inline__ int Is16bitAddress(void *address)
{
    return ((int) address >= (((FIRST_SELECTOR << 3) | 0x0007) << 16));
}

extern SEGDESC Segments[];

#endif /* SEGMEM_H */




