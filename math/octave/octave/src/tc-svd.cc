// tc-svd.cc                                           -*- C++ -*-
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

#include "Matrix.h"

#include "tree-const.h"
#include "user-prefs.h"
#include "gripes.h"
#include "error.h"

#ifdef WITH_DLD
tree_constant *
builtin_svd_2 (tree_constant *args, int nargin, int nargout)
{
  return svd (args, nargin, nargout);
}
#endif

tree_constant *
svd (tree_constant *args, int nargin, int nargout)
{
  tree_constant *retval = NULL_TREE_CONST;

  tree_constant arg = args[1].make_numeric ();

  if (arg.rows () == 0 || arg.columns () == 0)
    {
      int flag = user_pref.propagate_empty_matrices;
      if (flag != 0)
	{
	  if (flag < 0)
	    gripe_empty_arg ("svd", 0);
	  Matrix m;
	  retval = new tree_constant [4];
	  retval[0] = tree_constant (m);
	  retval[1] = tree_constant (m);
	  retval[2] = tree_constant (m);
	}
      else
	gripe_empty_arg ("svd", 1);

      return retval;
    }

  Matrix tmp;
  ComplexMatrix ctmp;
  switch (arg.const_type ())
    {
    case tree_constant_rep::scalar_constant:
      tmp.resize (1, 1);
      tmp.elem (0, 0) = arg.double_value ();
      break;
    case tree_constant_rep::matrix_constant:
      tmp = arg.matrix_value ();
      break;
    case tree_constant_rep::complex_scalar_constant:
      ctmp.resize (1, 1);
      ctmp.elem (0, 0) = arg.complex_value ();
      break;
    case tree_constant_rep::complex_matrix_constant:
      ctmp = arg.complex_matrix_value ();
      break;
    default:
      panic_impossible ();
      break;
    }

  switch (arg.const_type ())
    {
    case tree_constant_rep::scalar_constant:
    case tree_constant_rep::matrix_constant:
      {
	SVD result (tmp);

	DiagMatrix sigma = result.singular_values ();

	if (nargout == 1)
	  {
	    retval = new tree_constant [2];
	    retval[0] = tree_constant (sigma.diag (), 1);
	  }
	else
	  {
	    retval = new tree_constant [4];
	    retval[0] = tree_constant (result.left_singular_matrix ());
	    retval[1] = tree_constant (sigma);
	    retval[2] = tree_constant (result.right_singular_matrix ());
	  }
      }
      break;
    case tree_constant_rep::complex_scalar_constant:
    case tree_constant_rep::complex_matrix_constant:
      {
	ComplexSVD result (ctmp);

	DiagMatrix sigma = result.singular_values ();

	if (nargout == 1)
	  {
	    retval = new tree_constant [2];
	    retval[0] = tree_constant (sigma.diag (), 1);
	  }
	else
	  {
	    retval = new tree_constant [4];
	    retval[0] = tree_constant (result.left_singular_matrix ());
	    retval[1] = tree_constant (sigma);
	    retval[2] = tree_constant (result.right_singular_matrix ());
	  }
      }
      break;
    default:
      panic_impossible ();
      break;
    }

  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
