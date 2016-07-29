#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "includes/barreira.h"

void constroiArvore(nodeBarreira *pai, barreira *barr, int profundidade){
	if (profundidade == 0){
		barr->folha[barr->folhas] = pai;
		barr->folhas += 1;
	} else {
		int i;
		for (i = 0; i < barr->radix; i++){
			nodeBarreira *filho = malloc(sizeof(nodeBarreira));
			filho->contador = barr->radix;
			filho->pai = pai;
			filho->sense = false;

			constroiArvore(filho, barr, profundidade-1);
		}
	}
}

void inicializaBarreira(barreira *barr, int radix, int numFolhas){
	barr->radix = radix;
	barr->folhas = 0;
	barr->folha = malloc(sizeof(nodeBarreira) * numFolhas);
	int profundidade = 0;

	// iniciar threadSense ??

	while (numFolhas > 1){
		profundidade++;
		numFolhas = numFolhas / radix;
	}

	nodeBarreira *raiz = malloc(sizeof(nodeBarreira));
	raiz->contador = radix;
	raiz->pai = NULL;
	raiz->sense = false;

	constroiArvore(raiz, barr, profundidade-1);
}

void destroiBarreira(barreira *barreira){
	int i;
	for (i = 0; i < barreira->folhas; i++){
		nodeBarreira *atual = *barreira->folha;
		while (atual != NULL){
			nodeBarreira *prox = atual;
			atual = atual->pai;
			free(prox);
		}
	}
}

void barreiraWait(barreira *barreira){
	// usar thread sense;
}

void nodeWait(barreira *barreira, nodeBarreira *node){
	// usar thread sense;
}

