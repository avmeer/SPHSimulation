Alan VanderMeer and Zack Webb

Final Project: SPH and the Compute Kernel

This program runs a modestly-sized SPH fluid simulation in real time by using the compute shader to massively parallelize the particle simulation.

Usage:
  Q/A: Raise/lower the kernel's area of influence
  W/S: Raise/lower the multiplier of the kernel
  E/D: Raise/lower the multiplier of the gradient kernel
  R/F: Raise/lower the force of gravity
  T/G: Raise/lower the viscosity of the liquid
  Y/H: Raise/lower the influence of pressure
  Esc: Exit the program

Compiling:
The included makefile compiles on the CCIT lab computers. `mingw32-make run` will compile and run the program.

Bugs/Implementation Details:
The program is known to exhibit slightly different behavior depending on which machine it is running on. This may be due to the timekeeping method of each computer differing.

How long did this assignment take you? A while.

How fun was this assignment? 8
