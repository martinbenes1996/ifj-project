
declare function vypis() as integer

scope
  dim result as double
  result = vypis()

end scope

function vypis() as integer

  dim x as integer
  Input x

  dim i as integer
  Do WHile x>=0
    print x;
    x = x-1
  Loop

  return 10

end function
