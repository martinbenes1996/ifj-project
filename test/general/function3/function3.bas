
/'
  file:     function3.bas
  author:   xbenes49
  date:     28th november 2017
  Test of multiple parameters handling in functions.
'/

declare function add(a as double, b as double) as double

scope
  dim x as integer
  dim y as integer
  dim z as double

  Input x
  Input y

  z = add(x, y)

  PRINT z;
end scope

function add(x as double, b as double) as double
  return x+b
end function
