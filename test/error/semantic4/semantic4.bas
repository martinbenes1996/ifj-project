
/'
  file:     semantic2.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of not defined function.
'/

declare function f() as integer   'this function is only declared

scope
  dim x as integer
  x = f()
end scope
