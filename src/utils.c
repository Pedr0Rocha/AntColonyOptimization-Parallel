#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "estruturas.h"

void inicializaRandom(int seed){
	//srand(seed);
	srand(time(NULL));
}

double randomDouble(){
	return (double)rand() / (double)RAND_MAX;
}

// seleciona o filho usando roleta
node* selecaoRoleta(listaLigada *filhos, int qtaFilhos){
	double somatoriaFeromonio = 0;
	double pesosFilhos[qtaFilhos];
	int i = 0;

	listaLigada *atual = filhos;
	while (atual != NULL){
		somatoriaFeromonio += atual->nodeAtual->feromonio;
		pesosFilhos[i++] = atual->nodeAtual->feromonio;
		atual = atual->prev;
	}
	for (i = 1; i < qtaFilhos; i++)
		pesosFilhos[i] += pesosFilhos[i-1];

	double random = randomDouble();
	for (i = 0; i < qtaFilhos; i++){
		if (random <= pesosFilhos[i] / somatoriaFeromonio){
			printf("Filho Escolhido: %d\n", i);
			return getFilho(i, filhos);
		}
	}
	return NULL;

}