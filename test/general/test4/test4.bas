//
  file:     test4
  author:   xbenes49
  date:     28th november 2017
  Test of pre-declaration and after-definition.
//

declare function sq(i as double) as double

//
  main
//
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

//
  returns parameter i squared
//
function sq(x as double) as double

  'square
  return x*x

end function
