In order to compile the program:

1. Using terminal, navigate to the directory containing the program/C code

2. Execute the command ‘make’ in terminal

3. The program will be compiled as memsim.out (Mac) or memsim.exe (Windows)
Run this command in your terminal as follow:

To see tracking page loaded base on FIFO or LRU algorithm:

$ ./memsim <tracefile> <nframes> <fifo|lru> <quiet|debug> 
While tracefile is the name of file contain page number loaded, 
nframes is the nmuber of frames, fifo or lru is the policy,
quiet to see the output that how many disk reads and wirtes, 
and debug is to test the code.

To see tracking page loaded base on SFIFO algorithm:

$ ./memsim <tracefile> <nframes> <vms> <p> <quiet|debug> 
While tracefile is the name of file contain page number loaded, 
nframes is the nmuber of frames, vms is the SFIFO algorithm,
p (1-100) is the percentage of program memory sue in the secondary buffer,
quiet to see the output that how many disk reads and wirtes,
and debug is to test the code. 
