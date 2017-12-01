
/'
  file:     syntax2.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of nested functions.
'/

function f() as integer
  dim x as integer

  declare function() as integer

  return 0
  
end function

scope
  dim x as integer
  x = f()
end scope
