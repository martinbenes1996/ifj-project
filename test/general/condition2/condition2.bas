
/'
  file:     function2.bas
  author:   xbenes49
  date:     28th november 2017
  Test of complex conditions.
'/

function max3() as double
  dim a as double
  dim b as double
  dim c as double

  Input a
  Input b
  Input c

  if a > b then
    if a > c then

    'a is max
    return a

    else
      if c > b then

        'c is max
        return c
      else
      end if

    end if
  else

  end if

  if b > c then

    'b is max
    return b
  else
  end if

  'c is max
  return c

end function

scope

  dim result as double
  result = max3()

  Print result;

end scope
