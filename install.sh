#!/bin/bash

# You can use or not. Enables parallel compiling.
# $ ./install.sh -i     <- installation
# $ ./install.sh -u     <- uninstallation

# Then
# $ build               <- parallel make
# $ rebuild             <- make clean and parallel make

install() {
  # makes directory for user binaries
  mkdir -p ~/bin

  # install build
  rm -f ~/bin/build
  cp dev/scripts/build ~/bin/

  # install rebuild
  rm -f ~/bin/rebuild
  cp dev/scripts/rebuild ~/bin/

  # install run
  rm -f ~/bin/run
  cp dev/scripts/run ~/bin/
}

uninstall() {
  rm -f ~/bin/build
  rm -f ~/bin/rebuild
  rm -f ~/bin/run
}

if [ "$1" = "-i" ]; then
  echo "Installing..."
  install
elif [ "$1" = "-u" ]; then
  echo "Uninstalling..."
  uninstall
else
  printf "Give parameter:\n-i\tinstall\n-u\tuninstall\n"
fi
