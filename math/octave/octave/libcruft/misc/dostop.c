/* dostop.c                                              -*- C -*- */
/*

Copyright (C) 1992, 1993 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#include <stdlib.h>

/*
 * All the STOP statements in the Fortran routines have been replaced
 * with a call to XSTOPX, defined in the file libcruft/misc/xstopx.f.
 *
 * The XSTOPX function calls this function, which will send a SIGINT
 * signal to the program that invoked it.
 *
 * Octave\'s SIGINT signal handler calls jump_to_top_level(), and the
 * user will end up at the top level instead of the shell prompt.
 *
 * Programs that don\'t handle SIGINT will be interrupted.
 */

volatile void
#if defined (F77_APPEND_UNDERSCORE)
dostop_ (void)
#else
dostop (void)
#endif
{
  kill (getpid (), SIGINT);
  abort ();
}

/*
;;; Local Variables: ***
;;; mode: C ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
