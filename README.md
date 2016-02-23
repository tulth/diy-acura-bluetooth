# diy-acura-bluetooth
Overview
====================
My DIY project to understand, communicate, and add bluetooth with a 1999 Acura RL bose head unit

Cloning
====================
Be sure to include 

    --recursive 
  
during the git clone to get the submodules!

If you missed this, and do not want to reclone, do the following:

    git submodule update --init --recursive

Building
====================
Build platform:
Arch linux 
with community package: arm-none-eabi-gcc 5.3.0-1
with community package: arm-none-eabi-newlib
with package: python

Desktop shared library
------------
    cd target_desktop
    ./make.sh

Teensy3 embedded executables
------------
    cd target_teensy3
    ./make.sh


Loading
====================
To load you will need this arch linux AUR: teensy-loader-cli
