static char RCSId[] = "$Id: emulate.c,v 1.1 1994/02/24 08:05:20 hsu Exp $";
static char Copyright[] = "Copyright  Robert J. Amstadt, 1993";

#include "prototypes.h"
#include "regfunc.h"

struct Win87EmInfoStruct {
  unsigned short Version;
  unsigned short SizeSaveArea;
  unsigned short WinDataSeg;
  unsigned short WinCodeSeg;
  unsigned short Have80x87;
  unsigned short Unused;
};




int
WIN87_fpmath()
{
  printf( "_fpmath: (%d)\n",_BX);


  switch(_BX )
    {
    case 11:
      return 1;
    default:
      return 0;
    }

}

int
WIN87_WinEm87Info(struct Win87EmInfoStruct *pWIS, int cbWin87EmInfoStruct)
{
  printf( "__WinEm87Info(%p,%d)\n",pWIS,cbWin87EmInfoStruct);
}

int
WIN87_WinEm87Restore(void *pWin87EmSaveArea, int cbWin87EmSaveArea)
{
  printf( "__WinEm87Restore(%p,%d)\n",pWin87EmSaveArea,cbWin87EmSaveArea);
}

int
WIN87_WinEm87Save(void *pWin87EmSaveArea, int cbWin87EmSaveArea)
{
  printf( "__WinEm87Save(%p,%d)\n",pWin87EmSaveArea,cbWin87EmSaveArea);
}


