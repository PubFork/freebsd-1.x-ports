/*
 * Memory alllocation for the Wine Library toolkit
 *
 * Copyright (C) 1994 Miguel de Icaza
 *
 * All the memory management is being done by the libc malloc and friends.
 */

#ifndef __STDC__
#include <malloc.h>
#endif
#include "windows.h"

/* Controls the blocks per handle table */
#define MAXBLOCKS 512

static char Copyright [] = "Copyright (C) 1994 Miguel de Icaza";

typedef struct handle_table {
    struct handle_table *next;
    void *blocks [MAXBLOCKS];
} handle_table_t;

static handle_table_t handle_table;

static void **HEAP_GetFreeSlot (HANDLE *hNum)
{
    handle_table_t *table, *last;
    int i, j;

    for (table = &handle_table, j = 0; table; table = table->next, j++){
	for (i = 0; i < MAXBLOCKS; i++)
	    if (!table->blocks [i])
		goto AssignBlock;
	last = table;
    }

    /* No free slots */
    last->next = malloc (sizeof (handle_table_t));
    table = last->next;
    memset (table, 0, sizeof (handle_table_t));
    i = 0;
    
 AssignBlock:
    *hNum = j*MAXBLOCKS+i;
    return &table->blocks [i];
}

static void HEAP_Handle_is_Zero ()
{
    printf ("Warning: Handle is Zero, segmentation fault comming\n");
}

static void **HEAP_FindSlot (HANDLE hNum)
{
    handle_table_t *table = &handle_table;
    int i, j;

    if (!hNum)
	HEAP_Handle_is_Zero ();

    hNum--;
    for (j = hNum; j > MAXBLOCKS; j -= MAXBLOCKS){
	table = table->next;
	if (!table) return 0;
    }
    return &table->blocks [hNum%MAXBLOCKS];
}

HANDLE LocalAlloc (WORD flags, WORD bytes)
{
    void *m;
    void **slot;
    HANDLE hMem;

    slot = HEAP_GetFreeSlot (&hMem);
    if (flags & LMEM_WINE_ALIGN)
	m = memalign (4, bytes);
    else
	m = malloc (bytes);
    if (m){
	*slot = m;
	if (flags & LMEM_ZEROINIT)
	    bzero (m, bytes);

#ifdef DEBUG_HEAP
	printf ("Handle %d [%d] = %p\n", hMem+1, bytes, m);
#endif
	return hMem+1;
    }
    return 0;
}

WORD LocalCompact (WORD min_free)
{
    return min_free;
}

WORD LocalFlags (HANDLE hMem)
{
    return 0;
}

HANDLE LocalFree (HANDLE hMem)
{
    void **m = HEAP_FindSlot (hMem);

    free (*m);
    *m = 0;
    return 0;
}

BOOL LocalInit (WORD segment, WORD start, WORD end)
{
    return 1;
}

char *LocalLock (HANDLE hMem)
{
    void **m = HEAP_FindSlot (hMem);
#ifdef DEBUG_HEAP
    printf (">%d->%p\n", hMem, *m);
#endif
    return *m;
}

HANDLE LocalReAlloc (HANDLE hMem, WORD flags, WORD bytes)
{
    void **m = HEAP_FindSlot (hMem);

    realloc (*m, bytes);
}

WORD LocalSize (HANDLE hMem)
{
    /* Not implemented yet */
}


BOOL LocalUnLock (HANDLE hMem)
{
    return 0;
}

HANDLE GlobalAlloc (WORD flags, DWORD size)
{
    return LocalAlloc (flags, size);
}

HANDLE GlobalFree (HANDLE hMem)
{
    return LocalFree (hMem);
}

char *GlobalLock (HANDLE hMem)
{
    return LocalLock (hMem);
}

BOOL GlobalUnlock (HANDLE hMem)
{
    return LocalUnLock (hMem);
}

WORD GlobalFlags (HANDLE hMem)
{
    return LocalFlags (hMem);
}

DWORD GlobalSize (HANDLE hMem)
{
    return LocalSize (hMem);
}

DWORD GlobalCompact(DWORD desired)
{
    if (desired)
	return desired;
    else
	return 0x01000000;	/* Should check the available core. */
}

HANDLE GlobalReAlloc(HANDLE hMem, DWORD new_size, WORD flags)
{
    if (!(flags & GMEM_MODIFY))
	return LocalReAlloc (hMem, new_size, flags);
}

#ifdef UNIMPLEMENTED
void *GlobalQuickAlloc(int size)
{
}

DWORD int GlobalHandle(WORD selector)
{
}

unsigned int GlobalHandleFromPointer(void *block)
{
}

#endif
