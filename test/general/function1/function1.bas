
//
  file:     function1.bas
  author:   xbenes49
  date:     30th november 2017
  Test of pre-declaration and after-definition.
//

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
