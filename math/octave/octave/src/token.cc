// token.cc                                              -*- C++ -*-
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

#include "token.h"
#include "utils.h"
#include "symtab.h"

token::token (int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = generic_token;
}

token::token (char *s, int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = string_token;
  str = strsave (s);
}

token::token (double d, int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = double_token;
  num = d;
}

token::token (end_tok_type t, int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = ettype_token;
  et = t;
}

token::token (plot_tok_type t, int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = pttype_token;
  pt = t;
}

token::token (symbol_record *s, int l = -1, int c = -1)
{
  line_num = l;
  column_num = c;
  type_tag = sym_rec_token;
  sr = s;
}

token::~token (void)
{
  if (type_tag == string_token)
    delete [] str;
}

int
token::line (void)
{
  return line_num;
}

int
token::column (void)
{
  return column_num;
}

char *
token::string (void)
{
  assert (type_tag == string_token);
  return str;
}

double
token::number (void)
{
  assert (type_tag == double_token);
  return num;
}

token::end_tok_type
token::ettype (void)
{
  assert (type_tag == ettype_token);
  return et;
}

token::plot_tok_type
token::pttype (void)
{
  assert (type_tag == pttype_token);
  return pt;
}

symbol_record *
token::sym_rec (void)
{
  assert (type_tag == sym_rec_token);
  return sr;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
