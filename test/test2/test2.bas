
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

    end if

    'c is max
    return c

  end if

  'b is max
  return b

end function

scope

  dim result as double
  result = max3()

  Print result;

end scope
