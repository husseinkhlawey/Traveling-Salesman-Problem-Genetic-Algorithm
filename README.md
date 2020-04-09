# Traveling-Salesman-Problem-Genetic-Algorithm

Compile: mpiCC main.cpp -o main

Run: mpiexec -np 1 --hostfile \_hostfile main

Before running the main program, you will need to run the script.cpp to generate an
input file for main.cpp. This will server as the 100x100 grid. (Note: the same grid is
generated every time.)

Compile: g++ -o script script.cpp -g

Run: ./script

Timing and Results:

With the above mentioned grid and the adjusted parameters: m = 10,000, k = 1000, r = 20,
s = 0.75 * m, u = 0.1 * m, z = 0.05 * n, the following was achieved:

Shortest distance through the graph was ~1100 units, and ranging between 15 to 20 secs runtime.

Made by me for school project.
