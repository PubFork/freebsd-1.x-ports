// xpow.h                                               -*- C++ -*-
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

#if !defined (_xpow_h)
#define _xpow_h 1

#ifdef __GNUG__
#pragma interface
#endif

class Complex;
class Matrix;
class ComplexMatrix;
class tree_constant;

extern tree_constant xpow (double a, double b);
extern tree_constant xpow (double a, const Matrix& b);
extern tree_constant xpow (double a, const Complex& b);
extern tree_constant xpow (double a, const ComplexMatrix& b);

extern tree_constant xpow (const Matrix& a, double b);
extern tree_constant xpow (const Matrix& a, const Complex& b);

extern tree_constant xpow (const Complex& a, double b);
extern tree_constant xpow (const Complex& a, const Matrix& b);
extern tree_constant xpow (const Complex& a, const Complex& b);
extern tree_constant xpow (const Complex& a, const ComplexMatrix& b);

extern tree_constant xpow (const ComplexMatrix& a, double b);
extern tree_constant xpow (const ComplexMatrix& a, const Complex& b);

extern tree_constant elem_xpow (double a, const Matrix& b);
extern tree_constant elem_xpow (double a, const ComplexMatrix& b);

extern tree_constant elem_xpow (const Matrix& a, double b);
extern tree_constant elem_xpow (const Matrix& a, const Matrix& b);
extern tree_constant elem_xpow (const Matrix& a, const Complex& b);
extern tree_constant elem_xpow (const Matrix& a, const ComplexMatrix& b);

extern tree_constant elem_xpow (const Complex& a, const Matrix& b);
extern tree_constant elem_xpow (const Complex& a, const ComplexMatrix& b);

extern tree_constant elem_xpow (const ComplexMatrix& a, double b);
extern tree_constant elem_xpow (const ComplexMatrix& a, const Matrix& b);
extern tree_constant elem_xpow (const ComplexMatrix& a, const Complex& b);
extern tree_constant elem_xpow (const ComplexMatrix& a,
				const ComplexMatrix& b);

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
