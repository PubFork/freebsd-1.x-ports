#include <stdio.h>
#include <stdlib.h>
#include "msdos.h"
#include "wine.h"

static void Barf(struct sigcontext_struct *context)
{
	fprintf(stderr, "int10: unknown/not implemented parameters:\n");
	fprintf(stderr, "int10: AX %04x, BX %04x, CX %04x, DX %04x, "
	       "SI %04x, DI %04x, DS %04x, ES %04x\n",
	       AX, BX, CX, DX, SI, DI, DS, ES);
}

int do_int10(struct sigcontext_struct *context)
{
	switch((context->sc_eax >> 8) & 0xff)
	{
	case 0x0f:
		EAX = (EAX & 0xffffff00) | 0x5b;
		break;

	case 0x12:
		if ((EBX & 0xff) == 0x10) {
			EBX = (EBX & 0xffff0000) | 0x0003;
			ECX = (ECX & 0xffff0000) | 0x0009;
		}
		break;
			
	case 0x1a:
		EBX = (EBX & 0xffff0000) | 0x0008;
		break;
		
	default:
		Barf(context);
	};
	return 1;
}
