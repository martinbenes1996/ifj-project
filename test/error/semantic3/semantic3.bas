
/'
  file:     semantic3.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of semantic incompatibility.
'/

declare function f(x as string) as string

scope
  dim x as integer
  dim y as string
  y = f(x)            'semantic error
end scope

function f(x as string) as string
  return !"Hello"
end function
