// Helper functions for arithmetic operations.            -*- C++ -*-
// Used by the tree class.                    
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

#include <ctype.h>
#include <setjmp.h>
#include <math.h>
#include <Complex.h>

#include "error.h"
#include "gripes.h"
#include "utils.h"
#include "mappers.h"
#include "user-prefs.h"
#include "tree-const.h"
#include "arith-ops.h"
#include "unwind-prot.h"
#include "xpow.h"
#include "xdiv.h"

#if defined (HAVE_ISINF) || (defined (HAVE_FINITE) && defined (HAVE_ISNAN))
#define DIVIDE_BY_ZERO_ERROR \
  do \
    { \
      if (user_pref.warn_divide_by_zero) \
        warning ("division by zero"); \
    } \
  while (0)
#else
#define DIVIDE_BY_ZERO_ERROR \
  do \
    { \
      error ("division by zero attempted"); \
      return tree_constant (); \
    } \
  while (0)
#endif

// But first, some stupid functions that don\'t deserve to be in the
// Matrix class...

enum
Matrix_bool_op
{
  Matrix_LT,
  Matrix_LE,
  Matrix_EQ,
  Matrix_GE,
  Matrix_GT,
  Matrix_NE,
  Matrix_AND,
  Matrix_OR, 
};

/*
 * Check row and column dimensions for binary matrix operations.
 */
static inline int
m_add_conform (const Matrix& a, const Matrix& b, int warn)
{
  int ar = a.rows ();
  int ac = a.columns ();
  int br = b.rows ();
  int bc = b.columns ();

  int ok = (ar == br && ac == bc);

  if (! ok && warn)
    gripe_nonconformant (ar, ac, br, bc);

  return ok;
}

static inline int
m_add_conform (const Matrix& a, const ComplexMatrix& b, int warn)
{
  int ar = a.rows ();
  int ac = a.columns ();
  int br = b.rows ();
  int bc = b.columns ();

  int ok = (ar == br && ac == bc);

  if (! ok && warn)
    gripe_nonconformant (ar, ac, br, bc);

  return ok;
}

static inline int
m_add_conform (const ComplexMatrix& a, const Matrix& b, int warn)
{
  int ar = a.rows ();
  int ac = a.columns ();
  int br = b.rows ();
  int bc = b.columns ();

  int ok = (ar == br && ac == bc);

  if (! ok && warn)
    gripe_nonconformant (ar, ac, br, bc);

  return ok;
}

static inline int
m_add_conform (const ComplexMatrix& a, const ComplexMatrix& b, int warn)
{
  int ar = a.rows ();
  int ac = a.columns ();
  int br = b.rows ();
  int bc = b.columns ();

  int ok = (ar == br && ac == bc);

  if (! ok && warn)
    gripe_nonconformant (ar, ac, br, bc);

  return ok;
}

static inline int
m_mul_conform (const Matrix& a, const Matrix& b, int warn)
{
  int ac = a.columns ();
  int br = b.rows ();

  int ok = (ac == br);

  if (! ok && warn)
    gripe_nonconformant (a.rows (), ac, br, b.columns ());

  return ok;
}

static inline int
m_mul_conform (const Matrix& a, const ComplexMatrix& b, int warn)
{
  int ac = a.columns ();
  int br = b.rows ();

  int ok = (ac == br);

  if (! ok && warn)
    gripe_nonconformant (a.rows (), ac, br, b.columns ());

  return ok;
}

static inline int
m_mul_conform (const ComplexMatrix& a, const Matrix& b, int warn)
{
  int ac = a.columns ();
  int br = b.rows ();

  int ok = (ac == br);

  if (! ok && warn)
    gripe_nonconformant (a.rows (), ac, br, b.columns ());

  return ok;
}

static inline int
m_mul_conform (const ComplexMatrix& a, const ComplexMatrix& b, int warn)
{
  int ac = a.columns ();
  int br = b.rows ();

  int ok = (a.columns () == br);

  if (! ok && warn)
    gripe_nonconformant (a.rows (), ac, br, b.columns ());

  return ok;
}

/*
 * Stupid binary comparison operations like the ones Matlab provides.
 * One for each type combination, in the order given here:
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
 */

/* 1 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, double s, const Matrix& a)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = s < a.elem (i,j);
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = s <= a.elem (i,j);
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = s == a.elem (i,j);
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = s >= a.elem (i,j);
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = s > a.elem (i,j);
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = s != a.elem (i,j);
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = s && a.elem (i,j);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = s || a.elem (i,j);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 2 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, double s, const ComplexMatrix& a)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = s < real (a.elem (i,j));
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = s <= real (a.elem (i,j));
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = s == a.elem (i,j);
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = s >= real (a.elem (i,j));
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = s > real (a.elem (i,j));
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = s != a.elem (i,j);
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = s && (a.elem (i,j) != 0.0);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = s || (a.elem (i,j) != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 3 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Matrix& a, double s)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = a.elem (i,j) < s;
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = a.elem (i,j) <= s;
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = a.elem (i,j) == s;
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = a.elem (i,j) >= s;
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = a.elem (i,j) > s;
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = a.elem (i,j) != s;
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = a.elem (i,j) && s;
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = a.elem (i,j) || s;
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 4 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Matrix& a, const Complex& s)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = a.elem (i,j) < real (s);
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = a.elem (i,j) <= real (s);
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = a.elem (i,j) == s;
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = a.elem (i,j) >= real (s);
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = a.elem (i,j) > real (s);
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = a.elem (i,j) != s;
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = a.elem (i,j) && (s != 0.0);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = a.elem (i,j) || (s != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 5 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Matrix& a, const Matrix& b)
{
  if (! m_add_conform (a, b, 1))
    return Matrix ();
     
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix c (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    c.elem (i, j) = a.elem (i, j) <  b.elem (i, j);
	    break;
	  case Matrix_LE:
	    c.elem (i, j) = a.elem (i, j) <= b.elem (i, j);
	    break;
	  case Matrix_EQ:
	    c.elem (i, j) = a.elem (i, j) == b.elem (i, j);
	    break;
	  case Matrix_GE:
	    c.elem (i, j) = a.elem (i, j) >= b.elem (i, j);
	    break;
	  case Matrix_GT:
	    c.elem (i, j) = a.elem (i, j) >  b.elem (i, j);
	    break;
	  case Matrix_NE:
	    c.elem (i, j) = a.elem (i, j) != b.elem (i, j);
	    break;
	  case Matrix_AND:
	    c.elem (i, j) = a.elem (i, j) && b.elem (i, j);
	    break;
	  case Matrix_OR:
	    c.elem (i, j) = a.elem (i, j) || b.elem (i, j);
	    break;
	  default:
	    panic_impossible ();
	    break;
	  }
      }

  return c;
}

/* 6 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Matrix& a, const ComplexMatrix& b)
{
  if (! m_add_conform (a, b, 1))
    return Matrix ();
     
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix c (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    c.elem (i, j) = a.elem (i, j) <  real (b.elem (i, j));
	    break;
	  case Matrix_LE:
	    c.elem (i, j) = a.elem (i, j) <= real (b.elem (i, j));
	    break;
	  case Matrix_EQ:
	    c.elem (i, j) = a.elem (i, j) == b.elem (i, j);
	    break;
	  case Matrix_GE:
	    c.elem (i, j) = a.elem (i, j) >= real (b.elem (i, j));
	    break;
	  case Matrix_GT:
	    c.elem (i, j) = a.elem (i, j) >  real (b.elem (i, j));
	    break;
	  case Matrix_NE:
	    c.elem (i, j) = a.elem (i, j) != b.elem (i, j);
	    break;
	  case Matrix_AND:
	    c.elem (i, j) = a.elem (i, j) && (b.elem (i, j) != 0.0);
	    break;
	  case Matrix_OR:
	    c.elem (i, j) = a.elem (i, j) || (b.elem (i, j) != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	  }
      }
  return c;
}

/* 7 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Complex& s, const Matrix& a)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = real (s) < a.elem (i,j);
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = real (s) <= a.elem (i,j);
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = s == a.elem (i,j);
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = real (s) >= a.elem (i,j);
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = real (s) > a.elem (i,j);
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = s != a.elem (i,j);
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = (s != 0.0) && a.elem (i,j);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = (s != 0.0) || a.elem (i,j);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 8 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const Complex& s, const ComplexMatrix& a)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = real (s) < real (a.elem (i,j));
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = real (s) <= real (a.elem (i,j));
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = s == a.elem (i,j);
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = real (s) >= real (a.elem (i,j));
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = real (s) > real (a.elem (i,j));
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = s != a.elem (i,j);
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = (s != 0.0) && (a.elem (i,j) != 0.0);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = (s != 0.0) || (a.elem (i,j) != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 9 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const ComplexMatrix& a, double s)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = real (a.elem (i,j)) < s;
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = real (a.elem (i,j)) <= s;
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = a.elem (i,j) == s;
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = real (a.elem (i,j)) >= s;
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = real (a.elem (i,j)) > s;
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = a.elem (i,j) != s;
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = (a.elem (i,j) != 0.0) && s;
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = (a.elem (i,j) != 0.0) || s;
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 10 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const ComplexMatrix& a, const Complex& s)
{
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix t (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    t.elem (i,j) = real (a.elem (i,j)) < real (s);
	    break;
	  case Matrix_LE:
	    t.elem (i,j) = real (a.elem (i,j)) <= real (s);
	    break;
	  case Matrix_EQ:
	    t.elem (i,j) = a.elem (i,j) == s;
	    break;
	  case Matrix_GE:
	    t.elem (i,j) = real (a.elem (i,j)) >= real (s);
	    break;
	  case Matrix_GT:
	    t.elem (i,j) = real (a.elem (i,j)) > real (s);
	    break;
	  case Matrix_NE:
	    t.elem (i,j) = a.elem (i,j) != s;
	    break;
	  case Matrix_AND:
	    t.elem (i,j) = (a.elem (i,j) != 0.0) && (s != 0.0);
	    break;
	  case Matrix_OR:
	    t.elem (i,j) = (a.elem (i,j) != 0.0) || (s != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	}
    }

  return t;
}

/* 11 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const ComplexMatrix& a, const Matrix& b)
{
  if (! m_add_conform (a, b, 1))
    return Matrix ();
     
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix c (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    c.elem (i, j) = real (a.elem (i, j)) <  b.elem (i, j);
	    break;
	  case Matrix_LE:
	    c.elem (i, j) = real (a.elem (i, j)) <= b.elem (i, j);
	    break;
	  case Matrix_EQ:
	    c.elem (i, j) = a.elem (i, j) == b.elem (i, j);
	    break;
	  case Matrix_GE:
	    c.elem (i, j) = real (a.elem (i, j)) >= b.elem (i, j);
	    break;
	  case Matrix_GT:
	    c.elem (i, j) = real (a.elem (i, j)) >  b.elem (i, j);
	    break;
	  case Matrix_NE:
	    c.elem (i, j) = a.elem (i, j) != b.elem (i, j);
	    break;
	  case Matrix_AND:
	    c.elem (i, j) = (a.elem (i, j) != 0.0)  && b.elem (i, j);
	    break;
	  case Matrix_OR:
	    c.elem (i, j) = (a.elem (i, j) != 0.0) || b.elem (i, j);
	    break;
	  default:
	    panic_impossible ();
	    break;
	  }
      }
  return c;
}

/* 12 */
static Matrix
mx_stupid_bool_op (Matrix_bool_op op, const ComplexMatrix& a,
		   const ComplexMatrix& b) 
{
  if (! m_add_conform (a, b, 1))
    return Matrix ();
     
  int ar = a.rows ();
  int ac = a.columns ();

  Matrix c (ar, ac);

  for (int j = 0; j < ac; j++)
    for (int i = 0; i < ar; i++)
      {
	switch (op)
	  {
	  case Matrix_LT:
	    c.elem (i, j) = real (a.elem (i, j)) <  real (b.elem (i, j));
	    break;
	  case Matrix_LE:
	    c.elem (i, j) = real (a.elem (i, j)) <= real (b.elem (i, j));
	    break;
	  case Matrix_EQ:
	    c.elem (i, j) = a.elem (i, j) == b.elem (i, j);
	    break;
	  case Matrix_GE:
	    c.elem (i, j) = real (a.elem (i, j)) >= real (b.elem (i, j));
	    break;
	  case Matrix_GT:
	    c.elem (i, j) = real (a.elem (i, j)) >  real (b.elem (i, j));
	    break;
	  case Matrix_NE:
	    c.elem (i, j) = a.elem (i, j) != b.elem (i, j);
	    break;
	  case Matrix_AND:
	    c.elem (i, j) = (a.elem (i, j) != 0.0) && (b.elem (i, j) != 0.0);
	    break;
	  case Matrix_OR:
	    c.elem (i, j) = (a.elem (i, j) != 0.0) || (b.elem (i, j) != 0.0);
	    break;
	  default:
	    panic_impossible ();
	    break;
	  }
      }

  return c;
}

/*
 * Unary operations.  One for each numeric data type:
 *
 *   scalar
 *   complex_scalar
 *   matrix
 *   complex_matrix
 *
 */

tree_constant
do_unary_op (double d, tree::expression_type t)
{
  double result = 0.0;

  switch (t)
    {
    case tree::not:
      result = (! d);
      break;
    case tree::uminus:
      result = -d;
      break;
    case tree::hermitian:
    case tree::transpose:
      result = d;
      break;
    default:
      panic_impossible ();
      break;
    }

  return tree_constant (result);
}

tree_constant
do_unary_op (const Matrix& a, tree::expression_type t)
{
  Matrix result;

  switch (t)
    {
    case tree::not:
      result = (! a);
      break;
    case tree::uminus:
      result = -a;
      break;
    case tree::hermitian:
    case tree::transpose:
      result = a.transpose ();
      break;
    default:
      panic_impossible ();
      break;
    }

  return tree_constant (result);
}

tree_constant
do_unary_op (const Complex& c, tree::expression_type t)
{
  Complex result = 0.0;

  switch (t)
    {
    case tree::not:
      result = (c == 0.0);
      break;
    case tree::uminus:
      result = -c;
      break;
    case tree::hermitian:
      result = conj (c);
      break;
    case tree::transpose:
      result = c;
      break;
    default:
      panic_impossible ();
      break;
    }

  return tree_constant (result);
}

tree_constant
do_unary_op (const ComplexMatrix& a, tree::expression_type t)
{
  ComplexMatrix result;

  switch (t)
    {
    case tree::not:
      result = (! a);
      break;
    case tree::uminus:
      result = -a;
      break;
    case tree::hermitian:
      result = a.hermitian ();
      break;
    case tree::transpose:
      result = a.transpose ();
      break;
    default:
      panic_impossible ();
      break;
    }

  return tree_constant (result);
}

/*
 * Binary operations.  One for each type combination, in the order
 * given here:
 *
 *       op2 \ op1:   s   m   cs   cm
 *            +--   +---+---+----+----+
 *   scalar   |     | 1 | 5 | 9  | 13 |
 *                  +---+---+----+----+
 *   matrix         | 2 | 6 | 10 | 14 |
 *                  +---+---+----+----+
 *   complex_scalar | 3 | 7 | 11 | 15 |
 *                  +---+---+----+----+
 *   complex_matrix | 4 | 8 | 12 | 16 |
 *                  +---+---+----+----+
 */

/* 1 */
tree_constant
do_binary_op (double a, double b, tree::expression_type t)
{
  double result = 0.0;

  switch (t)
    {
    case tree::add:
      result = a + b;
      break;
    case tree::subtract:
      result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      if (b == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      result = a / b;
      break;
    case tree::leftdiv:
    case tree::el_leftdiv:
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      result = b / a;
      break;
    case tree::power:
    case tree::elem_pow:
      return xpow (a, b);
      break;
    case tree::cmp_lt:
      result = a < b;
      break;
    case tree::cmp_le:
      result = a <= b;
      break;
    case tree::cmp_eq:
      result = a == b;
      break;
    case tree::cmp_ge:
      result = a >= b;
      break;
    case tree::cmp_gt:
      result = a > b;
      break;
    case tree::cmp_ne:
      result = a != b;
      break;
    case tree::and:
      result = (a && b);
      break;
    case tree::or:
      result = (a || b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  return tree_constant (result);
}

/* 2 */
tree_constant
do_binary_op (double a, const Matrix& b, tree::expression_type t)
{
  Matrix result;

  switch (t)
    {
    case tree::add:
      result = a + b;
      break;
    case tree::subtract:
      result = a - b;
      break;
    case tree::el_leftdiv:
    case tree::leftdiv:
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      a = 1.0 / a;
// fall through...
    case tree::multiply:
    case tree::el_mul:
      result = a * b;
      break;
    case tree::el_div:
      return x_el_div (a, b);
      break;
    case tree::divide:
      gripe_nonconformant (1, 1, b.rows (), b.columns ());
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  return tree_constant (result);
}

/* 3 */
tree_constant
do_binary_op (double a, const Complex& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  double result = 0.0;
  Complex complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      if (b == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = a / b;
      break;
    case tree::leftdiv:
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = b / a;
      break;
    case tree::power:
    case tree::elem_pow:
      return xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = a < real (b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = a <= real (b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = a == b;
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = a >= real (b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = a > real (b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = a != b;
      break;
    case tree::and:
      result_type = RT_real;
      result = (a && (b != 0.0));
      break;
    case tree::or:
      result_type = RT_real;
      result = (a || (b != 0.0));
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 4 */
tree_constant
do_binary_op (double a, const ComplexMatrix& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::el_leftdiv:
    case tree::leftdiv:
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      a = 1.0 / a;
// fall through...
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::el_div:
      return x_el_div (a, b);
      break;
    case tree::divide:
      gripe_nonconformant (1, 1, b.rows (), b.columns ());
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 5 */
tree_constant
do_binary_op (const Matrix& a, double b, tree::expression_type t)
{
  Matrix result;

  switch (t)
    {
    case tree::add:
      result = a + b;
      break;
    case tree::subtract:
      result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result = a / b;
      break;
    case tree::el_leftdiv:
      return x_el_div (b, a);
      break;
    case tree::leftdiv:
      gripe_nonconformant (a.rows (), a.columns (), 1, 1);
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  return tree_constant (result);
}

/* 6 */
tree_constant
do_binary_op (const Matrix& a, const Matrix& b, tree::expression_type t)
{
  Matrix result;

  switch (t)
    {
    case tree::add:
      if (m_add_conform (a, b, 1))
	result = a + b;
      break;
    case tree::subtract:
      if (m_add_conform (a, b, 1))
	result = a - b;
      break;
    case tree::el_mul:
      if (m_add_conform (a, b, 1))
	result = a.product (b);
      break;
    case tree::multiply:
      if (m_mul_conform (a, b, 1))
	result = a * b;
      break;
    case tree::el_div:
      if (m_add_conform (a, b, 1))
	result = a.quotient (b);
      break;
    case tree::el_leftdiv:
      if (m_add_conform (a, b, 1))
	result = b.quotient (a);
      break;
    case tree::leftdiv:
      return xleftdiv (a, b);
      break;
    case tree::divide:
      return xdiv (a, b);
      break;
    case tree::power:
      error ("can't do A ^ B for A and B both matrices");
      break;
    case tree::elem_pow:
      if (m_add_conform (a, b, 1))
	return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  return tree_constant (result);
}

/* 7 */
tree_constant
do_binary_op (const Matrix& a, const Complex& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      complex_result = a / b;
      break;
    case tree::el_leftdiv:
      return x_el_div (b, a);
      break;
    case tree::leftdiv:
      gripe_nonconformant (a.rows (), a.columns (), 1, 1);
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 8 */
tree_constant
do_binary_op (const Matrix& a, const ComplexMatrix& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a - b;
      break;
    case tree::el_mul:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.product (b);
      break;
    case tree::multiply:
      result_type = RT_complex;
      if (m_mul_conform (a, b, 1))
	complex_result = a * b;
      break;
    case tree::el_div:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.quotient (b);
      break;
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = b.quotient (a);
      break;
    case tree::leftdiv:
      return xleftdiv (a, b);
      break;
    case tree::divide:
      return xdiv (a, b);
      break;
    case tree::power:
      error ("can't do A ^ B for A and B both matrices");
      break;
    case tree::elem_pow:
      if (m_add_conform (a, b, 1))
	return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 9 */
tree_constant
do_binary_op (const Complex& a, double b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  double result = 0.0;
  Complex complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      if (b == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = a / b;
      break;
    case tree::leftdiv:
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = b / a;
      break;
    case tree::power:
    case tree::elem_pow:
      return xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = real (a) < b;
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = real (a) <= b;
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = a == b;
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = real (a) >= b;
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = real (a) > b;
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = a != b;
      break;
    case tree::and:
      result_type = RT_real;
      result = ((a != 0.0) && b);
      break;
    case tree::or:
      result_type = RT_real;
      result = ((a != 0.0) || b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 10 */
tree_constant
do_binary_op (const Complex& a, const Matrix& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::el_leftdiv:
    case tree::leftdiv:
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      result_type = RT_complex;
      complex_result = b / a;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::el_div:
      return x_el_div (a, b);
      break;
    case tree::divide:
      gripe_nonconformant (1, 1, b.rows (), b.columns ());
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 11 */
tree_constant
do_binary_op (const Complex& a, const Complex& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  double result = 0.0;
  Complex complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      if (b == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = a / b;
      break;
    case tree::leftdiv:
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      complex_result = b / a;
      break;
    case tree::power:
    case tree::elem_pow:
      return xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = real (a) < real (b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = real (a) <= real (b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = a == b;
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = real (a) >= real (b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = real (a) > real (b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = a != b;
      break;
    case tree::and:
      result_type = RT_real;
      result = ((a != 0.0) && (b != 0.0));
      break;
    case tree::or:
      result_type = RT_real;
      result = ((a != 0.0) || (b != 0.0));
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 12 */
tree_constant
do_binary_op (const Complex& a, const ComplexMatrix& b,
	      tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::el_leftdiv:
    case tree::leftdiv:
      if (a == 0.0)
	DIVIDE_BY_ZERO_ERROR;
      result_type = RT_complex;
      complex_result = b / a;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::el_div:
      return x_el_div (a, b);
      break;
    case tree::divide:
      gripe_nonconformant (1, 1, b.rows (), b.columns ());
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 13 */
tree_constant
do_binary_op (const ComplexMatrix& a, double b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      complex_result = a / b;
      break;
    case tree::el_leftdiv:
      return x_el_div (b, a);
      break;
    case tree::leftdiv:
      gripe_nonconformant (a.rows (), a.columns (), 1, 1);
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 14 */
tree_constant
do_binary_op (const ComplexMatrix& a, const Matrix& b, tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a - b;
      break;
    case tree::el_mul:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.product (b);
      break;
    case tree::multiply:
      result_type = RT_complex;
      if (m_mul_conform (a, b, 1))
	complex_result = a * b;
      break;
    case tree::el_div:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.quotient (b);
      break;
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.quotient (b);
      break;
    case tree::leftdiv:
      return xleftdiv (a, b);
      break;
    case tree::divide:
      return xdiv (a, b);
      break;
    case tree::power:
      error ("can't do A ^ B for A and B both matrices");
      break;
    case tree::elem_pow:
      if (m_add_conform (a, b, 1))
	return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 15 */
tree_constant
do_binary_op (const ComplexMatrix& a, const Complex& b,
	      tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      complex_result = a - b;
      break;
    case tree::multiply:
    case tree::el_mul:
      result_type = RT_complex;
      complex_result = a * b;
      break;
    case tree::divide:
    case tree::el_div:
      result_type = RT_complex;
      complex_result = a / b;
      break;
    case tree::el_leftdiv:
      return x_el_div (b, a);
      break;
    case tree::leftdiv:
      gripe_nonconformant (a.rows (), a.columns (), 1, 1);
      break;
    case tree::power:
      return xpow (a, b);
      break;
    case tree::elem_pow:
      return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/* 16 */
tree_constant
do_binary_op (const ComplexMatrix& a, const ComplexMatrix& b,
	      tree::expression_type t)
{
  enum RT { RT_unknown, RT_real, RT_complex };
  RT result_type = RT_unknown;

  Matrix result;
  ComplexMatrix complex_result;

  switch (t)
    {
    case tree::add:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a + b;
      break;
    case tree::subtract:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a - b;
      break;
    case tree::el_mul:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.product (b);
      break;
    case tree::multiply:
      result_type = RT_complex;
      if (m_mul_conform (a, b, 1))
	complex_result = a * b;
      break;
    case tree::el_div:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = a.quotient (b);
      break;
    case tree::el_leftdiv:
      result_type = RT_complex;
      if (m_add_conform (a, b, 1))
	complex_result = b.quotient (a);
      break;
    case tree::leftdiv:
      return xleftdiv (a, b);
      break;
    case tree::divide:
      return xdiv (a, b);
      break;
    case tree::power:
      error ("can't do A ^ B for A and B both matrices");
      break;
    case tree::elem_pow:
      if (m_add_conform (a, b, 1))
	return elem_xpow (a, b);
      break;
    case tree::cmp_lt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LT, a, b);
      break;
    case tree::cmp_le:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_LE, a, b);
      break;
    case tree::cmp_eq:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_EQ, a, b);
      break;
    case tree::cmp_ge:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GE, a, b);
      break;
    case tree::cmp_gt:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_GT, a, b);
      break;
    case tree::cmp_ne:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_NE, a, b);
      break;
    case tree::and:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_AND, a, b);
      break;
    case tree::or:
      result_type = RT_real;
      if (m_add_conform (a, b, 1))
	result = mx_stupid_bool_op (Matrix_OR, a, b);
      break;
    default:
      panic_impossible ();
      break;
    }

  if (error_state)
    return tree_constant ();

  assert (result_type != RT_unknown);

  if (result_type == RT_real)
    return tree_constant (result);
  else
    return tree_constant (complex_result);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
