function retval = is_controllable (a, b, tol)

# Usage: is_controllable (a, b {,tol})
#
# Returns 1 if the pair (a, b) is controllable, or 0 if not.
#
# See also: size, rows, columns, length, is_matrix, is_scalar, is_vector
#
# This should really use the method below, but I'm being lazy for now:
#
# Controllability is determined by applying Arnoldi iteration with
# complete re-orthogonalization to obtain an orthogonal basis of the
# Krylov subspace.
#
# (FIX ME... The Krylov subspace approach is not done yet!)
#                      n-1
#   span ([b,a*b,...,a^   b]).
#
# tol is a roundoff paramter, set to 2*eps if omitted.

# Written by A. S. Hodel (scotte@eng.auburn.edu) August, 1993.

  if (nargin == 2 || nargin == 3)

    n = is_square (a);
    [nr, nc] = size (b);

    if (n == 0 || n != nr || nc == 0)
      retval = 0;
    else

      m = b;
      tmp = b;
      for ii = 1:(n-1)
        tmp = a * tmp;
        m = [m, tmp];
      endfor

# If n is of any significant size, m will be low rank, so be careful!

      if (nargin == 3)
        if (is_scalar (tol))
          retval = (rank (m, tol) == n);
        else
          error ("is_controllable: tol must be a scalar");
        endif
      else
        retval = (rank (m) == n);
      endif
    endif
  else
    error ("usage: is_controllable (a, b)");
  endif

endfunction
