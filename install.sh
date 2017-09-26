#!/bin/bash

# You can use or not. Enables parallel compiling.
# $ ./install.sh     <- installation

# Then
# $ build            <- parallel make
# $ rebuild          <- make clean and parallel make

# makes directory for user binaries
mkdir -p ~/bin

# installs new user scripts
if [ -e "~/bin/build" ]; then
  rm ~/bin/build
fi
cp dev/scripts/build ~/bin/
if [ -e "~/bin/rebuild" ]; then
  rm ~/bin/rebuild
fi
cp dev/scripts/rebuild ~/bin/
