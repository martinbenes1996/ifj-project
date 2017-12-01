
/'
  file:     semantic7.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of double definition.
'/

declare function f() as integer

scope
  dim x as integer
  x = f()
end scope

function f() as integer
  return 0
end function

function f() as integer
  return 0
end function
