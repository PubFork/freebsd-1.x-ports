function [xs, ys] = stairs (x, y)

# usage: [xs, ys] = stairs (x, y)
#
# Given two vectors of x-y data, stairs produces a `stairstep' plot.
#
# If only one argument is given, it is taken as a vector of y-values
# and the x coordiates are taken to be the indices of the elements.
#
# If two output arguments are specified, the data are generated but
# not plotted.  For example,
#
#   stairs (x, y);
#
# and
#
#   [xs, ys] = stairs (x, y);
#   plot (xs, ys);
#
# are equivalent.
#
# See also: plot, semilogx, semilogy, loglog, polar, mesh, contour,
#           bar, gplot, gsplot, replot, xlabel, ylabel, title 


  if (nargin == 1)
    if (is_vector (x))
      len = 2 * length (x);
      xs = ys = zeros (len, 1);
      k = 0;
      for i = 1:2:len
        xs(i) = k++;
        ys(i) = x(k);
        ys(i+1) = x(k);
        xs(i+1) = k;
      endfor
    else
      error ("stairs: argument must be a vector");
    endif
  elseif (nargin == 2)
    if (is_vector (x) && is_vector (y))
      xlen = length (x);
      ylen = length (y);
      if (xlen == ylen)
        len = 2 * xlen;
        xs = ys = zeros (len, 1);
	k = 1;
        len_m2 = len - 2;
	for i = 1:2:len_m2
	  xs(i) = x(k);
	  ys(i) = y(k);
	  ys(i+1) = y(k);
          k++;
	  xs(i+1) = x(k);
          if (x(k) < x(k-1))
            error ("stairs: x vector values must be in ascending order");
          endif
	endfor
        xs(len-1) = x(xlen);
        delta = x(xlen) - x(xlen-1);
        xs(len) = x(xlen) + delta;
        ys(len-1) = y(ylen);
        ys(len) = y(ylen);
      else
        error ("stairs: arguments must be the same length");
      endif
    else
      error ("stairs: arguments must be vectors");
    endif
  else
    error ("usage: [xs, ys] = stairs (x, y)");
  endif

  if (nargout == 1)
    plot (xs, ys);
  endif

endfunction
