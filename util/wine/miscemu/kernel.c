static char RCSId[] = "$Id: kernel.c,v 1.1 1994/02/24 08:05:21 hsu Exp $";
static char Copyright[] = "Copyright  Robert J. Amstadt, 1993";

#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"
#include "regfunc.h"
#include "options.h"

extern unsigned short WIN_StackSize;

/**********************************************************************
 *					KERNEL_LockSegment
 */
int
KERNEL_LockSegment(int segment)
{
    if (segment == -1)
	segment = *(Stack16Frame + 6);

#ifdef RELAY_DEBUG
    printf("LockSegment: segment %x\n", segment);
#endif

    return segment;
}

/**********************************************************************
 *					KERNEL_UnlockSegment
 */
int
KERNEL_UnlockSegment(int segment)
{
    if (segment == -1)
	segment = *(Stack16Frame + 6);

#ifdef RELAY_DEBUG
    printf("UnlockSegment: segment %x\n", segment);
#endif

    return segment;
}

/**********************************************************************
 *					KERNEL_InitTask
 */
KERNEL_InitTask()
{
    _BX = 0x81;
    _AX = 1;
    _CX = WIN_StackSize;
    _DX = Options.cmdShow;
    _SI = 0;
    ReturnFromRegisterFunc();
    /* Function does not return */
}

/**********************************************************************
 *					KERNEL_WaitEvent
 */
int
KERNEL_WaitEvent(int task)
{
#ifdef RELAY_DEBUG
    printf("WaitEvent: task %d\n", task);
#endif
    return 0;
}
