// xpow.cc                                               -*- C++ -*-
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

#include <assert.h>
#include "error.h"
#include "xpow.h"

// This function also appears in tree-const.cc.  Maybe it should be a
// member function of the Matrix class.

static int
any_element_is_negative (const Matrix& a)
{
  int nr = a.rows ();
  int nc = a.columns ();
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      if (a.elem (i, j) < 0.0)
	return 1;
  return 0;
}

/*
 * Safer pow functions.
 *
 *       op2 \ op1:   s   m   cs   cm
 *            +--   +---+---+----+----+
 *   scalar   |     | 1 | 5 |  7 | 11 |
 *                  +---+---+----+----+
 *   matrix         | 2 | E |  8 |  E |
 *                  +---+---+----+----+
 *   complex_scalar | 3 | 6 |  9 | 12 |
 *                  +---+---+----+----+
 *   complex_matrix | 4 | E | 10 |  E |
 *                  +---+---+----+----+
 *
 *   E -> error, trapped in arith-ops.cc.
 */

tree_constant
xpow (double a, double b)
{
  if (a < 0.0 && (int) b != b)
    {
      Complex atmp (a);
      return tree_constant (pow (atmp, b));
    }
  else
    return tree_constant (pow (a, b));
}

tree_constant
xpow (double a, Matrix& b)
{
  tree_constant retval;

  int nr = b.rows ();
  int nc = b.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    error ("for x^A, A must be square");
  else
    {
      EIG b_eig (b);
      ComplexColumnVector lambda (b_eig.eigenvalues ());
      ComplexMatrix Q (b_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	{
	  Complex elt = lambda.elem (i);
	  if (imag (elt) == 0.0)
	    lambda.elem (i) = pow (a, real (elt));
	  else
	    lambda.elem (i) = pow (a, elt);
	}
      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (double a, Complex& b)
{
  Complex result;
  Complex atmp (a);
  result = pow (atmp, b);
  return tree_constant (result);
}

tree_constant
xpow (double a, ComplexMatrix& b)
{
  tree_constant retval;

  int nr = b.rows ();
  int nc = b.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    error ("for x^A, A must be square");
  else
    {
      EIG b_eig (b);
      ComplexColumnVector lambda (b_eig.eigenvalues ());
      ComplexMatrix Q (b_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	{
	  Complex elt = lambda.elem (i);
	  if (imag (elt) == 0.0)
	    lambda.elem (i) = pow (a, real (elt));
	  else
	    lambda.elem (i) = pow (a, elt);
	}
      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (Matrix& a, double b)
{
  tree_constant retval;

  int nr = a.rows ();
  int nc = a.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    {
      error ("for A^b, A must be square");
      return retval;
    }

  if ((int) b == b)
    {
      int btmp = (int) b;
      if (btmp == 0)
	{
	  DiagMatrix result (nr, nr, 1.0);
	  retval = tree_constant (result);
	}
      else
	{
// Too much copying?
// XXX FIXME XXX -- we shouldn\'t do this if the exponent is large...
	  Matrix atmp;
	  if (btmp < 0)
	    {
	      btmp = -btmp;
	      atmp = a.inverse ();
	    }
	  else
	    atmp = a;

	  Matrix result (atmp);
	  for (int i = 1; i < btmp; i++)
	    result = result * atmp;

	  retval = tree_constant (result);
	}
    }
  else
    {
      EIG a_eig (a);
      ComplexColumnVector lambda (a_eig.eigenvalues ());
      ComplexMatrix Q (a_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	lambda.elem (i) = pow (lambda.elem (i), b);

      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (Matrix& a, Complex& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    {
      error ("for A^b, A must be square");
      return tree_constant ();
    }

  EIG a_eig (a);
  ComplexColumnVector lambda (a_eig.eigenvalues ());
  ComplexMatrix Q (a_eig.eigenvectors ());

  for (int i = 0; i < nr; i++)
    lambda.elem (i) = pow (lambda.elem (i), b);

  ComplexDiagMatrix D (lambda);

  ComplexMatrix result = Q * D * Q.inverse ();

  return tree_constant (result);
}

tree_constant
xpow (Complex& a, double b)
{
  Complex result;
  result = pow (a, b);
  return tree_constant (result);
}

tree_constant
xpow (Complex& a, Matrix& b)
{
  tree_constant retval;

  int nr = b.rows ();
  int nc = b.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    {
      error ("for x^A, A must be square");
    }
  else
    {
      EIG b_eig (b);
      ComplexColumnVector lambda (b_eig.eigenvalues ());
      ComplexMatrix Q (b_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	{
	  Complex elt = lambda.elem (i);
	  if (imag (elt) == 0.0)
	    lambda.elem (i) = pow (a, real (elt));
	  else
	    lambda.elem (i) = pow (a, elt);
	}
      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (Complex& a, Complex& b)
{
  Complex result;
  result = pow (a, b);
  return tree_constant (result);
}

tree_constant
xpow (Complex& a, ComplexMatrix& b)
{
  tree_constant retval;

  int nr = b.rows ();
  int nc = b.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    error ("for x^A, A must be square");
  else
    {
      EIG b_eig (b);
      ComplexColumnVector lambda (b_eig.eigenvalues ());
      ComplexMatrix Q (b_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	{
	  Complex elt = lambda.elem (i);
	  if (imag (elt) == 0.0)
	    lambda.elem (i) = pow (a, real (elt));
	  else
	    lambda.elem (i) = pow (a, elt);
	}
      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (ComplexMatrix& a, double b)
{
  tree_constant retval;

  int nr = a.rows ();
  int nc = a.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    {
      error ("for A^b, A must be square");
      return retval;
    }

  if ((int) b == b)
    {
      int btmp = (int) b;
      if (btmp == 0)
	{
	  DiagMatrix result (nr, nr, 1.0);
	  retval = tree_constant (result);
	}
      else
	{
// Too much copying?
// XXX FIXME XXX -- we shouldn\'t do this if the exponent is large...
	  ComplexMatrix atmp;
	  if (btmp < 0)
	    {
	      btmp = -btmp;
	      atmp = a.inverse ();
	    }
	  else
	    atmp = a;

	  ComplexMatrix result (atmp);
	  for (int i = 1; i < btmp; i++)
	    result = result * atmp;

	  retval = tree_constant (result);
	}
    }
  else
    {
      EIG a_eig (a);
      ComplexColumnVector lambda (a_eig.eigenvalues ());
      ComplexMatrix Q (a_eig.eigenvectors ());

      for (int i = 0; i < nr; i++)
	lambda.elem (i) = pow (lambda.elem (i), b);

      ComplexDiagMatrix D (lambda);

      ComplexMatrix result = Q * D * Q.inverse ();
      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
xpow (ComplexMatrix& a, Complex& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  if (nr == 0 || nc == 0 || nr != nc)
    {
      error ("for A^b, A must be square");
      return tree_constant ();
    }

  EIG a_eig (a);
  ComplexColumnVector lambda (a_eig.eigenvalues ());
  ComplexMatrix Q (a_eig.eigenvectors ());

  for (int i = 0; i < nr; i++)
    lambda.elem (i) = pow (lambda.elem (i), b);

  ComplexDiagMatrix D (lambda);

  ComplexMatrix result = Q * D * Q.inverse ();

  return tree_constant (result);
}

/*
 * Safer pow functions that work elementwise for matrices.
 *
 *       op2 \ op1:   s   m   cs   cm
 *            +--   +---+---+----+----+
 *   scalar   |     | * | 3 |  * |  9 |
 *                  +---+---+----+----+
 *   matrix         | 1 | 4 |  7 | 10 |
 *                  +---+---+----+----+
 *   complex_scalar | * | 5 |  * | 11 |
 *                  +---+---+----+----+
 *   complex_matrix | 2 | 6 |  8 | 12 |
 *                  +---+---+----+----+
 *
 *   * -> not needed.
 */

tree_constant
elem_xpow (double a, Matrix& b)
{
  tree_constant retval;

  int nr = b.rows ();
  int nc = b.columns ();

// For now, assume the worst.
  if (a < 0.0)
    {
      Complex atmp (a);
      ComplexMatrix result (nr, nc);
      for (int j = 0; j < nc; j++)
	for (int i = 0; i < nr; i++)
	  result.elem (i, j) = pow (atmp, b.elem (i, j));

      retval = tree_constant (result);
    }
  else
    {
      Matrix result (nr, nc);
      for (int j = 0; j < nc; j++)
	for (int i = 0; i < nr; i++)
	  result.elem (i, j) = pow (a, b.elem (i, j)); 

      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
elem_xpow (double a, ComplexMatrix& b)
{
  int nr = b.rows ();
  int nc = b.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a, b.elem (i, j));

  return tree_constant (result);
}

tree_constant
elem_xpow (Matrix& a, double b)
{
  tree_constant retval;

  int nr = a.rows ();
  int nc = a.columns ();

  if ((int) b != b && any_element_is_negative (a))
    {
      ComplexMatrix result (nr, nc);
      for (int j = 0; j < nc; j++)
	for (int i = 0; i < nr; i++)
	  {
	    Complex atmp (a.elem (i, j));
	    result.elem (i, j) = pow (atmp, b);
	  }

      retval = tree_constant (result);
    }
  else
    {
      Matrix result (nr, nc);
      for (int j = 0; j < nc; j++)
	for (int i = 0; i < nr; i++)
	  result.elem (i, j) = pow (a.elem (i, j), b);

      retval = tree_constant (result);
    }

  return retval;
}

tree_constant
elem_xpow (Matrix& a, Matrix& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  assert (nr == b.rows () && nc == b.columns ());

  int convert_to_complex = 0;
  int i;
  for (int j = 0; j < nc; j++)
    for (i = 0; i < nr; i++)
      {
	double atmp = a.elem (i, j);
	double btmp = b.elem (i, j);
	if (atmp < 0.0 && (int) btmp != btmp)
	  {
	    convert_to_complex = 1;
	    goto done;
	  }
      }

 done:

  if (convert_to_complex)
    {
      ComplexMatrix complex_result (nr, nc);

      for (j = 0; j < nc; j++)
	for (i = 0; i < nr; i++)
	  {
	    Complex atmp (a.elem (i, j));
	    Complex btmp (b.elem (i, j));
	    complex_result.elem (i, j) = pow (atmp, btmp);
	  }
      return tree_constant (complex_result);
    }
  else
    {
      Matrix result (nr, nc);

      for (j = 0; j < nc; j++)
	for (i = 0; i < nr; i++)
	  result.elem (i, j) = pow (a.elem (i, j), b.elem (i, j));

      return tree_constant (result);
    }
}

tree_constant
elem_xpow (Matrix& a, Complex& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b);

  return tree_constant (result);
}

tree_constant
elem_xpow (Matrix& a, ComplexMatrix& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  assert (nr == b.rows () && nc == b.columns ());

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b.elem (i, j));

  return tree_constant (result);
}

tree_constant
elem_xpow (Complex& a, Matrix& b)
{
  int nr = b.rows ();
  int nc = b.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a, b.elem (i, j));

  return tree_constant (result);
}

tree_constant
elem_xpow (Complex& a, ComplexMatrix& b)
{
  int nr = b.rows ();
  int nc = b.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a, b.elem (i, j));

  return tree_constant (result);
}

tree_constant
elem_xpow (ComplexMatrix& a, double b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b);

  return tree_constant (result);
}

tree_constant
elem_xpow (ComplexMatrix& a, Matrix& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  assert (nr == b.rows () && nc == b.columns ());

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b.elem (i, j));

  return tree_constant (result);
}

tree_constant
elem_xpow (ComplexMatrix& a, Complex& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  ComplexMatrix result (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b);

  return tree_constant (result);
}

tree_constant
elem_xpow (ComplexMatrix& a, ComplexMatrix& b)
{
  int nr = a.rows ();
  int nc = a.columns ();

  ComplexMatrix result (nr, nc);

  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      result.elem (i, j) = pow (a.elem (i, j), b.elem (i, j));

  return tree_constant (result);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
