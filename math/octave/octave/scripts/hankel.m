function retval = hankel (c, r)

# usage: hankel (c, r)
#
# Return the Hankel matrix constructed given the first column
# c, and (optionally) the last row r.
#
# If the second argument is omitted, the last row is taken to be the
# same as the first column.  If the last element of c is not the same
# as the first element of r, the last element of c is used.
#
# See also: vander, hadamard, hilb, invhilb, toeplitz

  if (nargin == 1)
    r = c;
  elseif (nargin != 2)
    error ("usage: hankel (c, r)");
  endif

  [c_nr, c_nc] = size (c);
  [r_nr, r_nc] = size (r);

  if ((c_nr != 1 && c_nc != 1) || (r_nr != 1 && r_nc != 1))
    error ("hankel: expecting vector arguments")
  endif

  if (c_nc != 1)
    c = c';
  endif

  if (r_nr != 1)
    r = r';
  endif

  if (r (1) != c (1))
    disp ("Column wins anti-diagonal conflict");
  endif

# This should probably be done with the colon operator...

  nc = length (r);
  nr = length (c);

  retval = zeros (nr, nc);

  for i = 1:min (nr, nc)
    retval (1:nr-i+1, i) = c (i:nr);
  endfor

  tmp = 1;
  if (nc <= nr)
    tmp = nr - nc + 2;
  endif

  for i = nr:-1:tmp
    retval (i, 2+nr-i:nc) = r (2:nc-nr+i);
  endfor

endfunction
