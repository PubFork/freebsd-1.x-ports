// unwind-prot.cc                                              -*- C++ -*-
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

#include <stddef.h>

#include "SLStack.h"

#include "Matrix.h"

#include "unwind-prot.h"
#include "error.h"
#include "utils.h"

class unwind_elem
{
 public:
  unwind_elem (void);
  unwind_elem (char *t);
  unwind_elem (cleanup_func f, void *p);
  unwind_elem (const unwind_elem& el);
  ~unwind_elem (void);

  unwind_elem& operator = (const unwind_elem& el);

  char *tag (void);
  cleanup_func fptr (void);
  void *ptr (void);

 private:
  char *_tag;
  cleanup_func _fptr;
  void *_ptr;
};

unwind_elem::unwind_elem (void)
{
  _tag = (char *) NULL;
  _fptr = (cleanup_func) NULL;
  _ptr = (void *) NULL;
}

unwind_elem::unwind_elem (char *t)
{
  _tag = strsave (t);
  _fptr = (cleanup_func) NULL;
  _ptr = (void *) NULL;
}

unwind_elem::unwind_elem (cleanup_func f, void *p)
{
  _tag = (char *) NULL;
  _fptr = f;
  _ptr = p;
}

unwind_elem::unwind_elem (const unwind_elem& el)
{
  _tag = strsave (el._tag);
  _fptr = el._fptr;
  _ptr = el._ptr;
}

unwind_elem::~unwind_elem (void)
{
  delete [] _tag;
}

unwind_elem&
unwind_elem::operator = (const unwind_elem& el)
{
  _tag = strsave (el._tag);
  _fptr = el._fptr;
  _ptr = el._ptr;

  return *this;
}

char *
unwind_elem::tag (void)
{
  return _tag;
}

cleanup_func
unwind_elem::fptr (void)
{
  return _fptr;
}

void *
unwind_elem::ptr (void)
{
  return _ptr;
}

static SLStack <unwind_elem> unwind_protect_list;

void
add_unwind_protect (cleanup_func fptr, void *ptr)
{
  unwind_elem el (fptr, ptr);
  unwind_protect_list.push (el);
}

void
run_unwind_protect (void)
{
  unwind_elem el = unwind_protect_list.pop ();

  cleanup_func f = el.fptr ();
  if (f != (cleanup_func) NULL)
    f (el.ptr ());
}

void
discard_unwind_protect (void)
{
  unwind_protect_list.pop ();
}

void
begin_unwind_frame (char *tag)
{
  unwind_elem elem (tag);
  unwind_protect_list.push (elem);
}

void
run_unwind_frame (char *tag)
{
  while (! unwind_protect_list.empty ())
    {
      unwind_elem el = unwind_protect_list.pop ();

      cleanup_func f = el.fptr ();
      if (f != (cleanup_func) NULL)
	f (el.ptr ());

      char *t = el.tag ();
      if (t != (char *) NULL && strcmp (t, tag) == 0)
	break;
    }
}

void
discard_unwind_frame (char *tag)
{
  while (! unwind_protect_list.empty ())
    {
      unwind_elem el = unwind_protect_list.pop ();
      char *t = el.tag ();
      if (t != (char *) NULL && strcmp (t, tag) == 0)
	break;
    }
}

void
run_all_unwind_protects (void)
{
  while (! unwind_protect_list.empty ())
    {
      unwind_elem el = unwind_protect_list.pop ();

      cleanup_func f = el.fptr ();
      if (f != (cleanup_func) NULL)
	f (el.ptr ());
    }
}

void
discard_all_unwind_protects (void)
{
  unwind_protect_list.clear ();
}

void
matrix_cleanup (void *m)
{
  delete [] (double *) m;
}

void
complex_matrix_cleanup (void *cm)
{
  delete [] (ComplexMatrix *) cm;
}

class saved_variable
{
 public:
  enum var_type { integer, generic_ptr, generic };

  saved_variable (void);
  saved_variable (int *p, int v);
  saved_variable (void **p, void *v);
  saved_variable (void *p, void *v, size_t sz);
  ~saved_variable (void);

  void restore_value (void);

 private:
  union
    {
      int *ptr_to_int;
      void *gen_ptr;
      void **ptr_to_gen_ptr;
    };

  union
    {
      int int_value;
      void *gen_ptr_value;
    };

  var_type type_tag;
  size_t size;
};

saved_variable::saved_variable (void)
{
  gen_ptr = (void *) NULL;
  gen_ptr_value = (void *) NULL;
  type_tag = generic;
  size = 0;
}

saved_variable::saved_variable (int *p, int v)
{
  type_tag = integer;
  ptr_to_int = p;
  int_value = v;
  size = sizeof (int);
}

saved_variable::saved_variable (void **p, void *v)
{
  type_tag = generic_ptr;
  ptr_to_gen_ptr = p;
  gen_ptr_value = v;
  size = sizeof (void *);
}

saved_variable::saved_variable (void *p, void *v, size_t sz)
{
  gen_ptr = v;
  gen_ptr_value = new char [sz];
  memcpy (gen_ptr_value, v, sz);
  size = sz;
}

saved_variable::~saved_variable (void)
{
  if (type_tag == generic)
    delete [] gen_ptr_value;
}

void
saved_variable::restore_value (void)
{
  switch (type_tag)
    {
    case integer:
      *ptr_to_int = int_value;
      break;
    case generic_ptr:
      *ptr_to_gen_ptr = gen_ptr_value;
      break;
    case generic:
      memcpy (gen_ptr, gen_ptr_value, size);
      break;
    default:
      panic_impossible ();
    }
}

static void
restore_saved_variable (void *s)
{
  saved_variable *sv = (saved_variable *) s;
  sv->restore_value ();
  delete sv;
}

void
unwind_protect_int_internal (int *ptr, int value)
{
  saved_variable *s = new saved_variable (ptr, value);
  add_unwind_protect (restore_saved_variable, (void *) s);
}

void
unwind_protect_ptr_internal (void **ptr, void *value)
{
  saved_variable *s = new saved_variable (ptr, value);
  add_unwind_protect (restore_saved_variable, (void *) s);
}

void
unwind_protect_var_internal (void *ptr, void *value, size_t size)
{
  saved_variable *s = new saved_variable (ptr, value, size);
  add_unwind_protect (restore_saved_variable, (void *) s);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
