// error.h                                              -*- C++ -*-
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

#if !defined (_error_h)
#define _error_h 1

#ifdef __GNUG__
#pragma interface
#endif

#define panic_impossible() \
  panic ("impossible state reached in file `%s' at line %d", \
	 __FILE__, __LINE__)

// Tell g++ that panic doesn't return;

#ifdef __GNUG__
typedef void v_fcn_cpc_x (const char *, ...);
volatile v_fcn_cpc_x panic;
#endif

extern void message (const char *name, const char *fmt, ...);
extern void usage (const char *fmt, ...);
extern void warning (const char *fmt, ...);
extern void error (const char *fmt, ...);
extern void panic (const char *fmt, ...);

// Current error state.
extern int error_state;

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
