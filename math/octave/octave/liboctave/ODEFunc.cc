// ODEFunc.cc                                            -*- C++ -*-
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

#include <iostream.h>
#include "ODEFunc.h"

ODEFunc::ODEFunc (void)
{
  fun = NULL;
  jac = NULL;
}

ODEFunc::ODEFunc (ODERHSFunc f)
{
  fun = f;
  jac = NULL;
}

ODEFunc::ODEFunc (ODERHSFunc f, ODEJacFunc j)
{
  fun = f;
  jac = j;
}

ODEFunc::ODEFunc (const ODEFunc& a)
{
  fun = a.function ();
  jac = a.jacobian_function ();
}

ODEFunc&
ODEFunc::operator = (const ODEFunc& a)
{
  fun = a.function ();
  jac = a.jacobian_function ();

  return *this;
}

ODERHSFunc
ODEFunc::function (void) const
{
  return fun;
}

ODEFunc&
ODEFunc::set_function (ODERHSFunc f)
{
  fun = f;
  return *this;
}

ODEJacFunc
ODEFunc::jacobian_function (void) const
{
  return jac;
}

ODEFunc&
ODEFunc::set_jacobian_function (ODEJacFunc j)
{
  jac = j;
  return *this;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
