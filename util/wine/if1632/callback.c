static char RCSId[] = "$Id: callback.c,v 1.1.1.2 1994/04/22 01:51:37 hsu Exp $";
static char Copyright[] = "Copyright  Robert J. Amstadt, 1993";

#include "windows.h"
#include "callback.h"
#include "wine.h"
#include "segmem.h"
#include <setjmp.h>

extern SEGDESC Segments[];
extern unsigned short IF1632_Saved16_ss;
extern unsigned long  IF1632_Saved16_ebp;
extern unsigned long  IF1632_Saved16_esp;
extern unsigned short IF1632_Saved32_ss;
extern unsigned long  IF1632_Saved32_ebp;
extern unsigned long  IF1632_Saved32_esp;

extern struct segment_descriptor_s *MakeProcThunks;

struct thunk_s
{
    int used;
    unsigned char thunk[10];
};

/**********************************************************************
 *					PushOn16
 */
static void
PushOn16(int size, unsigned int value)
{
    char *p = (char *) (((unsigned int)IF1632_Saved16_ss << 16) +
			(IF1632_Saved16_esp & 0xffff));
    if (size)
    {
	unsigned long *lp = (unsigned long *) p - 1;
	
	*lp = value;
	IF1632_Saved16_esp -= 4;
    }
    else
    {
	unsigned short *sp = (unsigned short *) p - 1;
	
	*sp = value;
	IF1632_Saved16_esp -= 2;
    }
}

/**********************************************************************
 *					FindDataSegmentForCode
 */
static unsigned short
FindDataSegmentForCode(unsigned long csip)
{
    unsigned int seg_idx;
    
    seg_idx = (unsigned short) (csip >> 19);
    return Segments[seg_idx].owner;
}

/**********************************************************************
 *					CallBack16
 */
int
CallBack16(void *func, int n_args, ...)
{
    va_list ap;
    int i;
    int arg_type, arg_value;
    
    va_start(ap, n_args);

    for (i = 0; i < n_args; i++)
    {
	arg_type = va_arg(ap, int);
	arg_value = va_arg(ap, int);
	PushOn16(arg_type, arg_value);
    }

    va_end(ap);

    return CallTo16((unsigned int) func, 
		    FindDataSegmentForCode((unsigned long) func));
}

/**********************************************************************
 *					CALLBACK_MakeProcInstance
 */
void *
CALLBACK_MakeProcInstance(void *func, int instance)
{
    int handle;
    void *new_func;
    struct thunk_s *tp;
    int i;
    
    tp = (struct thunk_s *) MakeProcThunks->base_addr;
    for (i = 0; i < 0x10000 / sizeof(*tp); i++, tp++)
	if (!tp->used)
	    break;
    
    if (tp->used)
	return (void *) 0;
    
    tp->thunk[0] = 0xb8;
    tp->thunk[1] = (unsigned char) instance;
    tp->thunk[2] = (unsigned char) (instance >> 8);
    tp->thunk[3] = 0x8e;
    tp->thunk[4] = 0xd8;
    tp->thunk[5] = 0xea;
    memcpy(&tp->thunk[6], &func, 4);
    tp->used = 1;

    return tp->thunk;
}

/**********************************************************************
 *					FreeProcInstance (KERNEL.52)
 */
void FreeProcInstance(FARPROC func)
{
    int handle;
    void *new_func;
    struct thunk_s *tp;
    int i;
    
    tp = (struct thunk_s *) MakeProcThunks->base_addr;
    for (i = 0; i < 0x10000 / sizeof(*tp); i++, tp++)
    {
	if ((void *) tp->thunk == (void *) func)
	{
	    tp->used = 0;
	    break;
	}
    }
}

/**********************************************************************
 *					CallWindowProc    (USER.122)
 */
LONG CallWindowProc( FARPROC func, HWND hwnd, WORD message,
		     WORD wParam, LONG lParam )
{
    SpyMessage(hwnd, message, wParam, lParam);
    
    if (Is16bitAddress(func))
    {	
	PushOn16( CALLBACK_SIZE_WORD, hwnd );
	PushOn16( CALLBACK_SIZE_WORD, message );
	PushOn16( CALLBACK_SIZE_WORD, wParam );
	PushOn16( CALLBACK_SIZE_LONG, lParam );
	return CallTo16((unsigned int) func, 
			FindDataSegmentForCode((unsigned long) func));   
    }
    else
    {
	return (*func)(hwnd, message, wParam, lParam);
    }
}

/**********************************************************************
 *					CallLineDDAProc
 */
void CallLineDDAProc(FARPROC func, short xPos, short yPos, long lParam)
{
    if (Is16bitAddress(func))
    {
	PushOn16( CALLBACK_SIZE_WORD, xPos );
	PushOn16( CALLBACK_SIZE_WORD, yPos );
	PushOn16( CALLBACK_SIZE_LONG, lParam );
	CallTo16((unsigned int) func, 
		 FindDataSegmentForCode((unsigned long) func));   
    }
    else
    {
	(*func)(xPos, yPos, lParam);
    }
}

/**********************************************************************
 *					CallHookProc
 */
DWORD CallHookProc( HOOKPROC func, short code, WPARAM wParam, LPARAM lParam )
{
    if (Is16bitAddress(func))
    {
	PushOn16( CALLBACK_SIZE_WORD, code );
	PushOn16( CALLBACK_SIZE_WORD, wParam );
	PushOn16( CALLBACK_SIZE_LONG, lParam );
	return CallTo16((unsigned int) func, 
			FindDataSegmentForCode((unsigned long) func));   
    }
    else
    {
	return (*func)( code, wParam, lParam );
    }
}

/**********************************************************************
 *					CallGrayStringProc
 */
BOOL CallGrayStringProc(FARPROC func, HDC hdc, LPARAM lParam, INT cch )
{
    if (Is16bitAddress(func))
    {
	PushOn16( CALLBACK_SIZE_WORD, hdc );
	PushOn16( CALLBACK_SIZE_LONG, lParam );
	PushOn16( CALLBACK_SIZE_WORD, cch );
	return CallTo16((unsigned int) func, 
			FindDataSegmentForCode((unsigned long) func));   
    }
    else
    {
	return (*func)( hdc, lParam, cch );
    }
}

/* ------------------------------------------------------------------------ */
/*
 * The following functions realize the Catch/Throw functionality.
 * My thought is to use the setjmp, longjmp combination to do the
 * major part of this one. All I have to remember, in addition to
 * whatever the jmp_buf contains, is the contents of the 16-bit
 * sp, bp and ss. I do this by storing them in the structure passed
 * to me by the 16-bit program (including my own jmp_buf...). 
 * Hopefully there isn't any program that modifies the contents!
 * Bad thing: I have to save part of the stack, since this will 
 * get reused on the next call after my return, leaving it in an
 * undefined state.
 */
#define	STACK_DEPTH_16 	28

struct special_buffer {
	jmp_buf buffer;
	long 	regs [6];
	char 	stack_part [STACK_DEPTH_16];
} *sb;

int Catch (LPCATCHBUF cbuf)
{
	WORD retval;
	jmp_buf *tmp_jmp;
	char *stack16 =  (char *) (((unsigned int)IF1632_Saved16_ss << 16) +
		(IF1632_Saved16_esp & 0xffff));

	sb = malloc (sizeof (struct special_buffer));
	
	sb -> regs [0] = IF1632_Saved16_esp;
	sb -> regs [1] = IF1632_Saved16_ebp;
	sb -> regs [2] = IF1632_Saved16_ss & 0xffff;
	sb -> regs [3] = IF1632_Saved32_esp;
	sb -> regs [4] = IF1632_Saved32_ebp;
	sb -> regs [5] = IF1632_Saved32_ss & 0xffff;
	memcpy (sb -> stack_part, stack16, STACK_DEPTH_16);
	tmp_jmp = &sb -> buffer;
	*((struct special_buffer **)cbuf) = sb;
	
	if ((retval = setjmp (*tmp_jmp)))
	{
		IF1632_Saved16_esp = sb -> regs [0];
		IF1632_Saved16_ebp = sb -> regs [1];
		IF1632_Saved16_ss = sb -> regs [2] & 0xffff;
		IF1632_Saved32_esp = sb -> regs [3];
		IF1632_Saved32_ebp = sb -> regs [4];
		IF1632_Saved32_ss = sb -> regs [5] & 0xffff;
		stack16 = (char *) (((unsigned int)IF1632_Saved16_ss << 16) +
				(IF1632_Saved16_esp & 0xffff));

		memcpy (stack16, sb -> stack_part, STACK_DEPTH_16);
#ifdef	DEBUG_CATCH
		printf ("Been thrown here: %d, retval = %d\n", sb, retval);
#endif
		free ((void *) sb);
		return (retval);
	} else {
#ifdef	DEBUG_CATCH
		printf ("Will somtime get thrown here: %d\n", sb);
#endif
		return (retval);
	}
}

void Throw (LPCATCHBUF cbuf, int val)
{
	sb = *((struct special_buffer **)cbuf);
#ifdef	DEBUG_CATCH
	printf ("Throwing to: %d\n", sb);
#endif
	longjmp (sb -> buffer, val);
}
