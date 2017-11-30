
/'
  file:     example1.bas
  author:   xbenes49
  date:     28th november 2017
  Test of example.
'/
scope
  Dim a As Integer
  DIM vysl AS INTEGER

  PrinT !"Zadejte cislo pro vypocet faktorialu";
  InpuT A
  If a < 0 THEN
    print !"\nFaktorial nelze spocitat";
  ELSE
    Vysl = 1
    Do WHile A > 0
      VYSL = vysl * a
      a = A - 1
    LooP
    Print !"\nVysledek je:" ; vYsl ; !"\n";
  end IF
END SCOPE
