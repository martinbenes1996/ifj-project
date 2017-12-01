
/'
  file:     semantic2.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of semantic incompatibility.
'/

declare function f() as string

scope
  dim x as integer
  x = f()            'semantic error
end scope

function f() as string
  return !"Hello"
end function
