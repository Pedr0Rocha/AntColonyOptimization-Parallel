ALL:
	mpicc -o mpi *.c -lm -O3 -std=c99
clear:
	rm mpi
run:
	mpirun -n 4 ./mpi -a 0.1 -b 1 -r 0.5 -n 1000 -f entradas/14mov.txt -c 1000
