
/'
  file:     semantic6.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of double declaration.
'/

declare function f() as integer

scope
  dim x as integer
  x = f()
end scope

declare function f() as integer

function f() as integer
  return 0
end function
