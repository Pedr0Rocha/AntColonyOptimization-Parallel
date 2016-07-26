ALL:
	mpicc *.c -lm -lpthread -O3 -ggdb -o a.out
clear:
	rm a.out
run:
	mpirun -n 2 ./a.out -a 0.1 -b 1 -r 0.5 -n 512 -f entradas/med/18mov.txt -c 6 -t 1