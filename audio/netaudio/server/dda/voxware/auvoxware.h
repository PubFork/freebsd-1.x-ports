/*
 * Copyright 1993 Network Computing Devices, Inc.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this
 * software without specific, written prior permission.
 * 
 * THIS SOFTWARE IS PROVIDED `AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * $NCDId: @(#)ausun.h,v 1.2 1993/11/10 21:18:32 greg Exp $
 */
/* modified by Amancio Hasty */
/* We are going to use the linux sound driver under FreeBSD */

#define	VENDOR_STRING		"VoxWare /dev/audio"
#define	VENDOR_RELEASE		1
#ifndef _FreeBSD_H_
#define _FreeBSD_H_

/*
 * NOTE: The native format endianess should match that of the machine
 * running the audio server.
 */

#define auNativeBytesPerSample	2


#define auNativeFormat		AuFormatLinearSigned16LSB


#define _BSD_SIGNALS

#include <signal.h>

typedef int AuBlock;
#define	AuUnBlockAudio(_x)	sigsetmask(_x)
#define	AuUnBlockAudio(_x)	sigsetmask(_x)
#define	AuBlockAudio()		sigblock(sigmask(SIGALRM))
#define AuProtectedMalloc(_s)	xalloc(_s)
#define AuProtectedFree(_p)	free(_p)

#endif /* !_FreeBSD_H_ */

