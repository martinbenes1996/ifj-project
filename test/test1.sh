#!/bin/bash

TDIR="test/test1/"
# compiling with tested compiler
./ifj < $TDIR"test1.bas" > $TDIR"test1_compiled"

# interpreting compiled result
./test/ic17int $TDIR"test1_compiled" < $TDIR"test1.stdin" > $TDIR"test1_compiled.stdout" 2> $TDIR"test1_compiled.stderr"
# interpreting right result
./test/ic17int $TDIR"test1.code" < $TDIR"test1.stdin" > $TDIR"test1.stdout" 2> $TDIR"test1.stderr"

# advanced method of compare
