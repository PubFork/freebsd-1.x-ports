// pr-output.cc                                               -*- C++ -*-
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
#include <strstream.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <Complex.h>

#include "Range.h"
#include "Matrix.h"

#include "tree-const.h"
#include "variables.h"
#include "user-prefs.h"
#include "pr-output.h"
#include "mappers.h"
#include "pager.h"
#include "error.h"
#include "utils.h"

// Current format string for real numbers and the real part of complex
// numbers.
static char *curr_real_fmt = (char *) NULL;

// Current format string for the imaginary part of complex numbers.
static char *curr_imag_fmt = (char *) NULL;

// Nonzero means don\'t do any fancy formatting.
static int free_format = 0;

// Nonzero means print plus sign for nonzero, blank for zero.
static int plus_format = 0;

// Nonzero means always print like dollars and cents.
static int bank_format = 0;

// Nonzero means use an e format.
static int print_e = 0;

// Nonzero means print E instead of e for exponent field.
static int print_big_e = 0;

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

static int
any_element_is_inf_or_nan (const Matrix& a)
{
  int nr = a.rows ();
  int nc = a.columns ();
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      {
	double val = a.elem (i, j);
	if (xisinf (val) || xisnan (val))
	  return 1;
      }
  return 0;
}

static int
any_element_is_inf_or_nan (const ComplexMatrix& a)
{
  int nr = a.rows ();
  int nc = a.columns ();
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      {
	Complex val = a.elem (i, j);
	if (xisinf (val) || xisnan (val))
	  return 1;
      }
  return 0;
}

static int
all_elements_are_int_or_inf_or_nan (const Matrix& a)
{
  int nr = a.rows ();
  int nc = a.columns ();
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      {
	double val = a.elem (i, j);
	if (xisnan (val) || D_NINT (val) == val)
	  continue;
	else
	  return 0;
      }
  return 1;
}

static Matrix
abs (const Matrix& a)
{
  int nr = a.rows ();
  int nc = a.columns ();
  Matrix retval (nr, nc);
  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      retval.elem (i, j) = fabs (a.elem (i, j));
  return retval;
}

static double
pr_max_internal (Matrix& m)
{
  int nr = m.rows ();
  int nc = m.columns ();

  double result = DBL_MIN;

  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      {
	double val = m.elem (i, j);
	if (xisinf (val) || xisnan (val))
	  continue;

	if (val > result)
	  result = val;
      }
  return result;
}

static double
pr_min_internal (Matrix& m)
{
  int nr = m.rows ();
  int nc = m.columns ();

  double result = DBL_MAX;

  for (int j = 0; j < nc; j++)
    for (int i = 0; i < nr; i++)
      {
	double val = m.elem (i, j);
	if (xisinf (val) || xisnan (val))
	  continue;

	if (val < result)
	  result = val;
      }
  return result;
}

static void
set_format (double d, int& fw)
{
  curr_real_fmt = (char *) NULL;
  curr_imag_fmt = (char *) NULL;

  if (free_format)
    return;

  static char fmt_buf[32];

  int sign = (d < 0.0);

  int inf_or_nan = (xisinf (d) || xisnan (d));

  double d_abs = d < 0.0 ? -d : d;

  int digits = d_abs == 0.0 ? 0 : (int) floor (log10 (d_abs) + 1.0);

  int prec = user_pref.output_precision;

  int ld, rd;

  if (bank_format)
    {
      fw = digits < 0 ? 4 : digits + 3;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
      rd = 2;
    }
  else if (xisnan (d) || D_NINT (d) == d)
    {
      fw = digits;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
      rd = 0;
    }
  else
    {
      if (digits > 0)
	{
	  ld = digits;
	  rd = prec - digits;
	  digits++;
	}
      else
	{
	  ld = 1;
	  rd = prec - digits;
	  digits = -digits + 1;
	}

      fw = ld + 1 + rd;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
    }

  if (! bank_format && (fw > user_pref.output_max_field_width || print_e))
    {
      int exp_field = 4;
      if (digits > 100)
	exp_field++;

      fw = 2 + prec + exp_field;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;

      if (print_big_e)
	sprintf (fmt_buf, "%%%d.%dE", fw, prec - 1);
      else
	sprintf (fmt_buf, "%%%d.%de", fw, prec - 1);
    }
  else
    {
      sprintf (fmt_buf, "%%%d.%df", fw, rd);
    }

  curr_real_fmt = &fmt_buf[0];
}

static inline void
set_format (double d)
{
  int fw;
  set_format (d, fw);
}

static void
set_format (Matrix& m, int& fw)
{
  curr_real_fmt = (char *) NULL;
  curr_imag_fmt = (char *) NULL;

  if (free_format)
    return;

  static char fmt_buf[32];

  int sign = any_element_is_negative (m);

  int inf_or_nan = any_element_is_inf_or_nan (m);

  Matrix m_abs = abs (m);
  double max_abs = pr_max_internal (m_abs);
  double min_abs = pr_min_internal (m_abs);

  int x_max = max_abs == 0.0 ? 0 : (int) floor (log10 (max_abs) + 1.0);
  int x_min = min_abs == 0.0 ? 0 : (int) floor (log10 (min_abs) + 1.0);

  int prec = user_pref.output_precision;

  int ld, rd;

  if (bank_format)
    {
      int digits = x_max > x_min ? x_max : x_min;
      fw = digits <= 0 ? 4 : digits + 3;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
      rd = 2;
    }
  else if (all_elements_are_int_or_inf_or_nan (m))
    {
      int digits = x_max > x_min ? x_max : x_min;
      fw = digits <= 0 ? 1 : digits;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
      rd = 0;
    }
  else
    {
      int ld_max, rd_max;
      if (x_max > 0)
	{
	  ld_max = x_max;
	  rd_max = prec - x_max;
	  x_max++;
	}
      else
	{
	  ld_max = 1;
	  rd_max = prec - x_max;
	  x_max = -x_max + 1;
	}

      int ld_min, rd_min;
      if (x_min > 0)
	{
	  ld_min = x_min;
	  rd_min = prec - x_min;
	  x_min++;
	}
      else
	{
	  ld_min = 1;
	  rd_min = prec - x_min;
	  x_min = -x_min + 1;
	}

      ld = ld_max > ld_min ? ld_max : ld_min;
      rd = rd_max > rd_min ? rd_max : rd_min;

      fw = ld + 1 + rd;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;
    }

  if (! bank_format && (fw > user_pref.output_max_field_width || print_e))
    {
      int exp_field = 4;
      if (x_max > 100 || x_min > 100)
	exp_field++;

      fw = 2 + prec + exp_field;
      if (inf_or_nan && fw < 3)
	fw = 3;
      fw += sign;

      if (print_big_e)
	sprintf (fmt_buf, "%%%d.%dE", fw, prec - 1);
      else
	sprintf (fmt_buf, "%%%d.%de", fw, prec - 1);
    }
  else
    {
      sprintf (fmt_buf, "%%%d.%df", fw, rd);
    }

  curr_real_fmt = &fmt_buf[0];
}

static inline void
set_format (Matrix& m)
{
  int fw;
  set_format (m, fw);
}

static void
set_format (Complex& c, int& r_fw, int& i_fw)
{
  curr_real_fmt = (char *) NULL;
  curr_imag_fmt = (char *) NULL;

  if (free_format)
    return;

  static char r_fmt_buf[32];
  static char i_fmt_buf[32];

  double rp = c.real ();
  double ip = c.imag ();

  int sign = (rp < 0.0);

  int inf_or_nan = (xisinf (c) || xisnan (c));

  double r_abs = rp < 0.0 ? -rp : rp;
  double i_abs = ip < 0.0 ? -ip : ip;

  int r_x = r_abs == 0.0 ? 0 : (int) floor (log10 (r_abs) + 1.0);
  int i_x = i_abs == 0.0 ? 0 : (int) floor (log10 (i_abs) + 1.0);

  int x_max, x_min;

  if (r_x > i_x)
    {
      x_max = r_x;
      x_min = i_x;
    }
  else
    {
      x_max = i_x;
      x_min = r_x;
    }

  int prec = user_pref.output_precision;

  int ld, rd;

  if (bank_format)
    {
      int digits = r_x;
      i_fw = 0;
      r_fw = digits <= 0 ? 4 : digits + 3;
      if (inf_or_nan && r_fw < 3)
	r_fw = 3;
      r_fw += sign;
      rd = 2;
    }
  else if (inf_or_nan || (D_NINT (rp) == rp && D_NINT (ip) == ip))
    {
      int digits = x_max > x_min ? x_max : x_min;
      i_fw = r_fw = digits <= 0 ? 1 : digits;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;
      rd = 0;
    }
  else
    {
      int ld_max, rd_max;
      if (x_max > 0)
	{
	  ld_max = x_max;
	  rd_max = prec - x_max;
	  x_max++;
	}
      else
	{
	  ld_max = 1;
	  rd_max = prec - x_max;
	  x_max = -x_max + 1;
	}

      int ld_min, rd_min;
      if (x_min > 0)
	{
	  ld_min = x_min;
	  rd_min = prec - x_min;
	  x_min++;
	}
      else
	{
	  ld_min = 1;
	  rd_min = prec - x_min;
	  x_min = -x_min + 1;
	}

      ld = ld_max > ld_min ? ld_max : ld_min;
      rd = rd_max > rd_min ? rd_max : rd_min;

      i_fw = r_fw = ld + 1 + rd;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;
    }

  if (! bank_format && (r_fw > user_pref.output_max_field_width || print_e))
    {
      int exp_field = 4;
      if (x_max > 100 || x_min > 100)
	exp_field++;

      i_fw = r_fw = 1 + prec + exp_field;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;

      if (print_big_e)
	{
	  sprintf (r_fmt_buf, "%%%d.%dE", r_fw, prec - 1);
	  sprintf (i_fmt_buf, "%%%d.%dE", i_fw, prec - 1);
	}
      else
	{
	  sprintf (r_fmt_buf, "%%%d.%de", r_fw, prec - 1);
	  sprintf (i_fmt_buf, "%%%d.%de", i_fw, prec - 1);
	}
    }
  else
    {
      sprintf (r_fmt_buf, "%%%d.%df", r_fw, rd);
      sprintf (i_fmt_buf, "%%%d.%df", i_fw, rd);
    }

  curr_real_fmt = &r_fmt_buf[0];
  curr_imag_fmt = &i_fmt_buf[0];
}

static inline void
set_format (Complex& c)
{
  int r_fw, i_fw;
  set_format (c, r_fw, i_fw);
}

static void
set_format (ComplexMatrix& cm, int& r_fw, int& i_fw)
{
  curr_real_fmt = (char *) NULL;
  curr_imag_fmt = (char *) NULL;

  if (free_format)
    return;

  static char r_fmt_buf[32];
  static char i_fmt_buf[32];

  Matrix rp = real (cm);
  Matrix ip = imag (cm);

  int sign = any_element_is_negative (rp);

  int inf_or_nan = any_element_is_inf_or_nan (cm);

  Matrix r_m_abs = abs (rp);
  double r_max_abs = pr_max_internal (r_m_abs);
  double r_min_abs = pr_min_internal (r_m_abs);

  Matrix i_m_abs = abs (ip);
  double i_max_abs = pr_max_internal (i_m_abs);
  double i_min_abs = pr_min_internal (i_m_abs);

  int r_x_max = r_max_abs == 0.0 ? 0 : (int) floor (log10 (r_max_abs) + 1.0);
  int r_x_min = r_min_abs == 0.0 ? 0 : (int) floor (log10 (r_min_abs) + 1.0);

  int i_x_max = i_max_abs == 0.0 ? 0 : (int) floor (log10 (i_max_abs) + 1.0);
  int i_x_min = i_min_abs == 0.0 ? 0 : (int) floor (log10 (i_min_abs) + 1.0);

  int x_max = r_x_max > i_x_max ? r_x_max : i_x_max;
  int x_min = r_x_min > i_x_min ? r_x_min : i_x_min;

  int prec = user_pref.output_precision;

  int ld, rd;

  if (bank_format)
    {
      int digits = r_x_max > r_x_min ? r_x_max : r_x_min;
      i_fw = 0;
      r_fw = digits <= 0 ? 4 : digits + 3;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;
      rd = 2;
    }
  else if (all_elements_are_int_or_inf_or_nan (rp)
	   && all_elements_are_int_or_inf_or_nan (ip))
    {
      int digits = x_max > x_min ? x_max : x_min;
      i_fw = r_fw = digits <= 0 ? 1 : digits;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;
      rd = 0;
    }
  else
    {
      int ld_max, rd_max;
      if (x_max > 0)
	{
	  ld_max = x_max;
	  rd_max = prec - x_max;
	  x_max++;
	}
      else
	{
	  ld_max = 1;
	  rd_max = prec - x_max;
	  x_max = -x_max + 1;
	}

      int ld_min, rd_min;
      if (x_min > 0)
	{
	  ld_min = x_min;
	  rd_min = prec - x_min;
	  x_min++;
	}
      else
	{
	  ld_min = 1;
	  rd_min = prec - x_min;
	  x_min = -x_min + 1;
	}

      ld = ld_max > ld_min ? ld_max : ld_min;
      rd = rd_max > rd_min ? rd_max : rd_min;

      i_fw = r_fw = ld + 1 + rd;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;
    }

  if (! bank_format && (r_fw > user_pref.output_max_field_width || print_e))
    {
      int exp_field = 4;
      if (x_max > 100 || x_min > 100)
	exp_field++;

      i_fw = r_fw = 1 + prec + exp_field;
      if (inf_or_nan && i_fw < 3)
	i_fw = r_fw = 3;
      r_fw += sign;

      if (print_big_e)
	{
	  sprintf (r_fmt_buf, "%%%d.%dE", r_fw, prec - 1);
	  sprintf (i_fmt_buf, "%%%d.%dE", i_fw, prec - 1);
	}
      else
	{
	  sprintf (r_fmt_buf, "%%%d.%de", r_fw, prec - 1);
	  sprintf (i_fmt_buf, "%%%d.%de", i_fw, prec - 1);
	}
    }
  else
    {
      sprintf (r_fmt_buf, "%%%d.%df", r_fw, rd);
      sprintf (i_fmt_buf, "%%%d.%df", i_fw, rd);
    }

  curr_real_fmt = &r_fmt_buf[0];
  curr_imag_fmt = &i_fmt_buf[0];
}

static int
all_elements_are_ints (Range& r)
{
// If the base and increment are ints, the final value in the range
// will also be an integer, even if the limit is not.

  double b = r.base ();
  double i = r.inc ();

  return ((double) NINT (b) == b && (double) NINT (i) == i);
}

static inline void
set_format (ComplexMatrix& cm)
{
  int r_fw, i_fw;
  set_format (cm, r_fw, i_fw);
}

static void
set_format (Range& r, int& fw)
{
  curr_real_fmt = (char *) NULL;
  curr_imag_fmt = (char *) NULL;

  if (free_format)
    return;

  static char fmt_buf[32];

  double r_min = r.base ();
  double r_max = r.limit ();

  if (r_max < r_min)
    {
      double tmp = r_max;
      r_max = r_min;
      r_min = tmp;
    }

  int sign = (r_min < 0.0);

  double max_abs = r_max < 0.0 ? -r_max : r_max;
  double min_abs = r_min < 0.0 ? -r_min : r_min;

  int x_max = max_abs == 0.0 ? 0 : (int) floor (log10 (max_abs) + 1.0);
  int x_min = min_abs == 0.0 ? 0 : (int) floor (log10 (min_abs) + 1.0);

  int prec = user_pref.output_precision;

  int ld, rd;

  if (bank_format)
    {
      int digits = x_max > x_min ? x_max : x_min;
      fw = sign + digits < 0 ? 4 : digits + 3;
      rd = 2;
    }
  else if (all_elements_are_ints (r))
    {
      int digits = x_max > x_min ? x_max : x_min;
      fw = sign + digits;
      rd = 0;
    }
  else
    {
      int ld_max, rd_max;
      if (x_max > 0)
	{
	  ld_max = x_max;
	  rd_max = prec - x_max;
	  x_max++;
	}
      else
	{
	  ld_max = 1;
	  rd_max = prec - x_max;
	  x_max = -x_max + 1;
	}

      int ld_min, rd_min;
      if (x_min > 0)
	{
	  ld_min = x_min;
	  rd_min = prec - x_min;
	  x_min++;
	}
      else
	{
	  ld_min = 1;
	  rd_min = prec - x_min;
	  x_min = -x_min + 1;
	}

      ld = ld_max > ld_min ? ld_max : ld_min;
      rd = rd_max > rd_min ? rd_max : rd_min;

      fw = sign + ld + 1 + rd;
    }

  if (! bank_format && (fw > user_pref.output_max_field_width || print_e))
    {
      int exp_field = 4;
      if (x_max > 100 || x_min > 100)
	exp_field++;

      fw = sign + 2 + prec + exp_field;

      if (print_big_e)
	sprintf (fmt_buf, "%%%d.%dE", fw, prec - 1);
      else
	sprintf (fmt_buf, "%%%d.%de", fw, prec - 1);
    }
  else
    {
      sprintf (fmt_buf, "%%%d.%df", fw, rd);
    }

  curr_real_fmt = &fmt_buf[0];
}

static inline void
set_format (Range& r)
{
  int fw;
  set_format (r, fw);
}

static inline void
pr_any_float (char *fmt, ostrstream& os, double d, int fw = 0)
{
  if (d == -0.0)
    d = 0.0;

  if (fmt == (char *) NULL)
    os << d;
  else
    {
      if (xisinf (d))
	{
	  char *s;
	  if (d < 0.0)
	    s = "-Inf";
	  else
	    s = "Inf";

	  if (fw > 0)
	    os.form ("%*s", fw, s);
	  else
	    os << s;
	}
      else if (xisnan (d))
	{
	  if (fw > 0)
	    os.form ("%*s", fw, "NaN");
	  else
	    os << "NaN";
	}
      else
	os.form (fmt, d);
    }
}

static inline void
pr_float (ostrstream& os, double d, int fw = 0)
{
  pr_any_float (curr_real_fmt, os, d, fw);
}

static inline void
pr_imag_float (ostrstream& os, double d, int fw = 0)
{
  pr_any_float (curr_imag_fmt, os, d, fw);
}

static inline void
pr_complex (ostrstream& os, Complex& c, int r_fw = 0, int i_fw = 0)
{
  double r = c.real ();
  pr_float (os, r, r_fw);
  if (! bank_format)
    {
      double i = c.imag ();
      if (i < 0)
	{
	  os << " - ";
	  i = -i;
	  pr_imag_float (os, i, i_fw);
	}
      else
	{
	  os << " + ";
	  pr_imag_float (os, i, i_fw);
	}
      os << "i";
    }
}

void
octave_print_internal (ostrstream& os, double d)
{
  if (plus_format)
    {
      if (d == 0.0)
	os << " ";
      else
	os << "+";
    }
  else
    {
      set_format (d);
      if (free_format)
	os << d;
      else
	pr_float (os, d);
    }
  os << "\n";
}

void
octave_print_internal (ostrstream& os, Matrix& m)
{
  int nr = m.rows ();
  int nc = m.columns ();

  if (plus_format)
    {
      for (int i = 0; i < nr; i++)
	{
	  for (int j = 0; j < nc; j++)
	    {
	      if (j == 0)
		os << "  ";

	      if (m.elem (i, j) == 0.0)
		os << " ";
	      else
		os << "+";
	    }
	  os << "\n";
	}
    }
  else
    {
      int fw;
      set_format (m, fw);
      int column_width = fw + 2;
      int total_width = nc * column_width;
      int max_width = terminal_columns ();

      if (free_format)
	{
	  os << m;
	  return;
	}

      int inc = nc;
      if (total_width > max_width && user_pref.split_long_rows)
	{
	  inc = max_width / column_width;
	  if (inc == 0)
	    inc++;
	}

      int col = 0;
      while (col < nc)
	{
	  int lim = col + inc < nc ? col + inc : nc;

	  if (total_width > max_width && user_pref.split_long_rows)
	    {
	      if (col != 0)
		os << "\n";

	      int num_cols = lim - col;
	      if (num_cols == 1)
		os << " Column " << col + 1 << ":\n\n";
	      else if (num_cols == 2)
		os << " Columns " << col + 1 << " and " << lim << ":\n\n";
	      else
		os << " Columns " << col + 1 << " through " << lim << ":\n\n";
	    }

	  for (int i = 0; i < nr; i++)
	    {
	      for (int j = col; j < lim; j++)
		{
		  os << "  ";
		  pr_float(os, m.elem (i, j), fw);
		}
	      os << "\n";
	    }
	  col += inc;
	}
    }
}

void
octave_print_internal (ostrstream& os, Complex& c)
{
  if (plus_format)
    {
      if (c == 0.0)
	os << " ";
      else
	os << "+";
    }
  else
    {
      set_format (c);
      if (free_format)
	os << c;
      else
	pr_complex (os, c);
    }
  os << "\n";
}

void
octave_print_internal (ostrstream& os, ComplexMatrix& cm)
{
  int nr = cm.rows ();
  int nc = cm.columns ();

  if (plus_format)
    {
      for (int i = 0; i < nr; i++)
	{
	  for (int j = 0; j < nc; j++)
	    {
	      if (j == 0)
		os << "  ";

	      if (cm.elem (i, j) == 0.0)
		os << " ";
	      else
		os << "+";
	    }
	  os << "\n";
	}
    }
  else
    {
      int r_fw, i_fw;
      set_format (cm, r_fw, i_fw);
      int column_width = i_fw + r_fw;
      column_width += bank_format ? 2 : 7;
      int total_width = nc * column_width;
      int max_width = terminal_columns ();

      if (free_format)
	{
	  os << cm;
	  return;
	}

      int inc = nc;
      if (total_width > max_width && user_pref.split_long_rows)
	{
	  inc = max_width / column_width;
	  if (inc == 0)
	    inc++;
	}

      int col = 0;
      while (col < nc)
	{
	  int lim = col + inc < nc ? col + inc : nc;

	  if (total_width > max_width && user_pref.split_long_rows)
	    {
	      if (col != 0)
		os << "\n";

	      int num_cols = lim - col;
	      if (num_cols == 1)
		os << " Column " << col + 1 << ":\n\n";
	      else if (num_cols == 2)
		os << " Columns " << col + 1 << " and " << lim << ":\n\n";
	      else
		os << " Columns " << col + 1 << " through " << lim << ":\n\n";
	    }

	  for (int i = 0; i < nr; i++)
	    {
	      for (int j = col; j < lim; j++)
		{
		  if (bank_format)
		    os << "  ";
		  else
		    os << "   ";
		  pr_complex (os, cm.elem (i, j), r_fw, i_fw);
		}
	      os << "\n";
	    }
	  col += inc;
	}
    }
}

void
octave_print_internal (ostrstream& os, Range& r)
{
  double b = r.base ();
  double increment = r.inc ();
  int num_elem = r.nelem ();

  if (plus_format)
    {
      os << "  ";
      for (int i = 0; i < num_elem; i++)
	{
	  double val = b + i * increment;
	  if (val == 0.0)
	    os << " ";
	  else
	    os << "+";
	}
    }
  else
    {
      int fw;
      set_format (r, fw);
      int column_width = fw + 2;
      int total_width = num_elem * column_width;
      int max_width = terminal_columns ();

      if (free_format)
	{
	  os << r;
	  return;
	}

      int inc = num_elem;
      if (total_width > max_width && user_pref.split_long_rows)
	{
	  inc = max_width / column_width;
	  if (inc == 0)
	    inc++;
	}

      int col = 0;
      while (col < num_elem)
	{
	  int lim = col + inc < num_elem ? col + inc : num_elem;

	  if (total_width > max_width && user_pref.split_long_rows)
	    {
	      if (col != 0)
		os << "\n";

	      int num_cols = lim - col;
	      if (num_cols == 1)
		os << " Column " << col + 1 << ":\n\n";
	      else if (num_cols == 2)
		os << " Columns " << col + 1 << " and " << lim << ":\n\n";
	      else
		os << " Columns " << col + 1 << " through " << lim << ":\n\n";
	    }

	  for (int i = col; i < lim; i++)
	    {
	      double val = b + i * increment;
	      os << "  ";
	      pr_float (os, val, fw);
	    }

	  os << "\n";

	  col += inc;
	}
    }
}

static void
init_format_state (void)
{
  free_format = 0;
  plus_format = 0;
  bank_format = 0;
  print_e = 0;
  print_big_e = 0;
}

static void
set_output_prec_and_fw (int prec, int fw)
{
  tree_constant *tmp = NULL_TREE_CONST;

  tmp = new tree_constant ((double) prec);
  bind_variable ("output_precision", tmp);

  tmp = new tree_constant ((double) fw);
  bind_variable ("output_max_field_width", tmp);
}

void
set_format_style (int argc, char **argv)
{
  if (--argc > 0)
    {
      argv++;
      if (*argv[0])
	{
	  if (strcmp (*argv, "short") == 0)
	    {
	      if (--argc > 0)
		{
		  argv++;
		  if (strcmp (*argv, "e") == 0)
		    {
		      init_format_state ();
		      print_e = 1;
		    }
		  else if (strcmp (*argv, "E") == 0)
		    {
		      init_format_state ();
		      print_e = 1;
		      print_big_e = 1;
		    }
		  else
		    {
		      message ("format",
			       "unrecognized option `short %s'", *argv);
		      return;
		    }
		}
	      else
		init_format_state ();

	      set_output_prec_and_fw (3, 8);
	    }
	  else if (strcmp (*argv, "long") == 0)
	    {
	      if (--argc > 0)
		{
		  argv++;
		  if (strcmp (*argv, "e") == 0)
		    {
		      init_format_state ();
		      print_e = 1;
		    }
		  else if (strcmp (*argv, "E") == 0)
		    {
		      init_format_state ();
		      print_e = 1;
		      print_big_e = 1;
		    }
		  else
		    {
		      message ("format",
			       "unrecognized option `long %s'", *argv);
		      return;
		    }
		}
	      else
		init_format_state ();

	      set_output_prec_and_fw (15, 24);
	    }
	  else if (strcmp (*argv, "hex") == 0)
	    message ("format", "format state `hex' not implemented");
	  else if (strcmp (*argv, "+") == 0)
	    {
	      init_format_state ();
	      plus_format = 1;
	    }
	  else if (strcmp (*argv, "bank") == 0)
	    {
	      init_format_state ();
	      bank_format = 1;
	    }
	  else if (strcmp (*argv, "free") == 0)
	    {
	      init_format_state ();
	      free_format = 1;
	    }
	  else if (strcmp (*argv, "none") == 0)
	    {
	      init_format_state ();
	      free_format = 1;
	    }
	  else if (strcmp (*argv, "compact") == 0)
	    message ("format", "format state `compact' not implemented");
	  else if (strcmp (*argv, "loose") == 0)
	    message ("format", "format state `loose' not implemented");
	  else
	    message ("format", "unrecognized format state `%s'", *argv);
	}
      else
	usage ("format [format_state]");
    }
  else
    {
      init_format_state ();
      set_output_prec_and_fw (5, 10);
    }
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
