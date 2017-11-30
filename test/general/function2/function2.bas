
/'
  file:     function2.bas
  author:   xbenes49
  date:     28th november 2017
  Test of parameter handling in functions.
'/

declare function sq(i as double) as double

scope
  'integer input
  dim x as integer
  Input x

  'count
  dim sq_x as double
  sq_x = sq(x)

  'show result
  Print sq_x;

end scope

function sq(x as double) as double

  'square
  return x*x

end function
