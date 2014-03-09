Making MakeFiles
----------------

In this module, we go over how to create a basic makefile, then proceed to creating 
functional makefiles for the atmega328p.

We will conclude with going over a makefile built by Google professionals (yes, Google does robotics).


Go over the basics of Make Files from here:

http://mrbook.org/tutorials/make/



make has flags and targets.


Set the Compiler Flags at the beginning:

`CFLAGS=-Wall -O3 -lm` etc.

Go over individual flag meanings, and the spirit of C (execution speed above all else)

`make` by default chooses the "All target"

but you can also create other convenient functions.

Most common is `make clean` which removes the old executables

If you are creating a binary for adding to the command line, you can write a target called:
`make install`, keep in mind these names are more or less standard, but besides `make all` entirely up to you.

Any command line argument can go into the tabbed section, so if you wanted to put a `figlet Done | cowsay -vf dragon` on the last line, you could.
