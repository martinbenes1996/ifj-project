
/'
  file:     string3.bas
  author:   xbenes49
  date:     28th november 2017
  Test of string from int.
'/

scope
  dim str as string
  dim c as string

  c = chr(97) 'a
  str += c

  c = chr(104) 'h
  str += c

  c = chr(111) 'o
  str += c

  c = chr(106) 'j
  str += c

  Print str;

end scope
