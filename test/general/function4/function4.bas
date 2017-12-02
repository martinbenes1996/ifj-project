
/'
  file:     function4.bas
  author:   xbenes49
  date:     28th november 2017
  Test of implicit return.
'/

function f() as integer
end function

scope
  dim x as integer = 4
  x = f()
  print x;
end scope
