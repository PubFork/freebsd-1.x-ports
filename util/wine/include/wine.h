#ifndef  WINE_H
#define  WINE_H

extern char *WineIniFileName(void);
extern char *WinIniFileName(void);

#define WINE_INI WineIniFileName()
#define WIN_INI WinIniFileName()

#ifdef linux
struct sigcontext_struct {
	unsigned short sc_gs, __gsh;
	unsigned short sc_fs, __fsh;
	unsigned short sc_es, __esh;
	unsigned short sc_ds, __dsh;
	unsigned long sc_edi;
	unsigned long sc_esi;
	unsigned long sc_ebp;
	unsigned long sc_esp;
	unsigned long sc_ebx;
	unsigned long sc_edx;
	unsigned long sc_ecx;
	unsigned long sc_eax;
	unsigned long sc_trapno;
	unsigned long sc_err;
	unsigned long sc_eip;
	unsigned short sc_cs, __csh;
	unsigned long sc_efl;
	unsigned long esp_at_signal;
	unsigned short sc_ss, __ssh;
	unsigned long i387;
	unsigned long oldmask;
	unsigned long cr2;
};
#endif

#ifdef linux
#define WINE_DATA_SELECTOR 0x2b
#define WINE_CODE_SELECTOR 0x23
#endif
#if defined(__NetBSD__) || defined(__FreeBSD__)
#define WINE_DATA_SELECTOR 0x27
#define WINE_CODE_SELECTOR 0x1f
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__)
#include <signal.h>
#define sigcontext_struct sigcontext
#define HZ 100
#endif

#endif /* WINE_H */
