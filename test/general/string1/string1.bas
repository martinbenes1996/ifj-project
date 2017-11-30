
/'
  file:     string1.bas
  author:   xbenes49
  date:     28th november 2017
  Test of string concatenation.
 '/

scope
  dim x as string = !"Hello"
  x += !", world!\n"
  Print x;
end scope
