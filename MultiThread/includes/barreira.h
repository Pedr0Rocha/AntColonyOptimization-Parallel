#ifndef BARREIRA_H
#define BARREIRA_H

#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef struct nodeBarreira nodeBarreira;

struct nodeBarreira{
	atomic_int contador;
	nodeBarreira *pai;
	volatile bool sense;	
};

typedef struct{
	int radix;
	nodeBarreira **folha;
	int folhas;
} barreira;

void inicializaBarreira(barreira *barreira, int radix, int numFolhas);
void barreiraWait(barreira *barreira);
void destroiBarreira(barreira *barreira);

#endif