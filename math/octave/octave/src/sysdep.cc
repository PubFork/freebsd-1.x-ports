// sysdep.cc                                              -*- C++ -*-
/*

Copyright (C) 1993 John W. Eaton

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

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>

#include "error.h"

#ifdef __386BSD__
# include <floatingpoint.h>
# define FP_ALL (FP_X_OFL | FP_X_INV | FP_X_DZ | FP_X_UFL| FP_X_DNML | FP_X_IMP)
#endif

#ifdef NeXT
extern "C"
{
  typedef void (*_cplus_fcn_int) (int);
  extern void (*malloc_error (_cplus_fcn_int)) (int);
}

static void
malloc_handler (int code)
{
  if (code == 5)
    message ("hopefully recoverable malloc error: freeing wild pointer");
  else
    {
      panic ("probably irrecoverable malloc error: code %d", code);
    }
}

static void
NeXT_init (void)
{
  malloc_error (malloc_handler);
}
#endif

void
sysdep_init (void)
{
#ifdef __386BSD__
/* disable trapping on all exceptions */
	fpsetmask(~(FP_ALL));
#endif
#ifdef NeXT
  NeXT_init ();
#endif
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
