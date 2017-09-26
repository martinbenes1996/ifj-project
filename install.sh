#!/bin/bash

# You can use or not. Enables parallel compiling.
# $ ./install.sh     <- installation

# Then
# $ build            <- parallel make
# $ rebuild          <- make clean and parallel make

# makes directory for user binaries
if [ ! -e "~/bin" ]; then
  mkdir ~/bin
fi

# installs new user scripts
cp dev/scripts/build ~/bin/
cp dev/scripts/rebuild ~/bin/
