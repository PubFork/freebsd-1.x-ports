static char RCSId[] = "$Id: user.c,v 1.1 1994/02/24 08:05:21 hsu Exp $";
static char Copyright[] = "Copyright  Robert J. Amstadt, 1993";

#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"
#include "windows.h"
#include "user.h"
#include "message.h"

#define USER_HEAP_SIZE          0x10000


MDESC *USER_Heap = NULL;


extern BOOL ATOM_Init();
extern BOOL GDI_Init();
extern void SYSMETRICS_Init();

/***********************************************************************
 *           USER_HeapInit
 */
static BOOL USER_HeapInit()
{
    struct segment_descriptor_s * s;
    s = GetNextSegment( 0, 0x10000 );
    if (s == NULL) return FALSE;
    HEAP_Init( &USER_Heap, s->base_addr, USER_HEAP_SIZE );
    return TRUE;
}


/**********************************************************************
 *					USER_InitApp
 *
 * Load necessary resources?
 */
int
USER_InitApp(int hInstance)
{
    int queueSize;

    SpyInit();

      /* Global atom table initialisation */
    if (!ATOM_Init()) return 0;
    
      /* GDI initialisation */
    if (!GDI_Init()) return 0;

      /* Initialize system colors and metrics*/
    SYSMETRICS_Init();
    SYSCOLOR_Init();
    
      /* Create USER heap */
    if (!USER_HeapInit()) return 0;
    
      /* Create the DCEs */
    DCE_Init();
    
      /* Initialize built-in window classes */
    if (!WIDGETS_Init()) return 0;

      /* Initialize dialog manager */
    if (!DIALOG_Init()) return 0;

      /* Create system message queue */
    queueSize = GetProfileInt( "windows", "TypeAhead", 120 );
    if (!MSG_CreateSysMsgQueue( queueSize )) return 0;

      /* Create task message queue */
    queueSize = GetProfileInt( "windows", "DefaultQueueSize", 8 );
    if (!SetMessageQueue( queueSize )) return 0;
        
    return 1;
}
