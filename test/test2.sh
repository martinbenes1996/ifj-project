TDIR="test/test2/"
# compiling with tested compiler
./ifj < $TDIR"test2.bas" > $TDIR"test2_compiled"

# interpreting compiled result
./test/ic17int $TDIR"test2_compiled" < $TDIR"test2.stdin" > $TDIR"test2_compiled.stdout" 2> $TDIR"test2_compiled.stderr"
# interpreting right result
./test/ic17int $TDIR"test2.code" < $TDIR"test2.stdin" > $TDIR"test2.stdout" 2> $TDIR"test2.stderr"

# advanced method of compare
