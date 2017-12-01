
/'
  file:     syntax1.bas
  author:   xbenes49
  date:     1st december 2017
  Error test of blocks.
'/

scope
  dim x as integer

  if x < 10 then
    do while x <> 0
      x += 1
    end if
  loop
end scope
