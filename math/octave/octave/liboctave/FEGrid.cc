// FEGrid.cc                                              -*- C++ -*-
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "FEGrid.h"

// error handling

void
FEGrid::error (const char* msg) const
{
  cerr << "Fatal FEGrid error. " << msg << "\n";
  exit(1);
}

void
FEGrid::nel_error (void) const
{
  error ("number of elements less than 1");
}

// Constructors

FEGrid::FEGrid (int nel, double width)
{
  if (nel < 1)
    nel_error ();

  elem.resize (nel+1);

  for (int i = 0; i <= nel; i++)
    elem.elem (i) = i * width;
}

FEGrid::FEGrid (int nel, double left, double right)
{
  if (nel < 1)
    nel_error ();

  elem.resize (nel+1);

  double width = (right - left) / (double) nel;

  for (int i = 0; i <= nel; i++)
    elem.elem (i) = i * width + left;

  check_grid ();
}

int
FEGrid::element (double x) const
{
  if (! in_bounds (x))
    error ("value not within grid boundaries");

  int nel = elem.capacity () - 1;
  for (int i = 1; i <= nel; i++)
    {
      if (x >= elem.elem (i-1) && x <= elem.elem (i))
	return i;
    }
  return -1;
       
}

void
FEGrid::check_grid (void) const
{
  int nel = elem.capacity () - 1;
  if (nel < 1)
    nel_error ();

  for (int i = 1; i <= nel; i++)
    {
      if (elem.elem (i-1) > elem.elem (i))
	error ("element boundaries not in ascending order");

      if (elem.elem (i-1) == elem.elem (i))
	error ("zero width element");
    }
}

ostream&
operator << (ostream& s, const FEGrid& g)
{
  s << g.element_boundaries ();
  return s;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
