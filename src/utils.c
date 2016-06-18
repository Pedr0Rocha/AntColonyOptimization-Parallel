#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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
node* selecaoRoleta(listaLigada *filhos, int qtaFilhos, double alfa, double beta){
	double somatoriaProb = 0;
	double pesosFilhos[qtaFilhos];
	int i = 0;

	listaLigada *atual = filhos;
	while (atual != NULL){
		somatoriaProb += (pow(atual->nodeAtual->feromonio, alfa) * pow(atual->nodeAtual->valorHeuristica, beta));
		pesosFilhos[i++] = (pow(atual->nodeAtual->feromonio, alfa) * pow(atual->nodeAtual->valorHeuristica, beta));
		atual = atual->prev;
	}
	for (i = 1; i < qtaFilhos; i++)
		pesosFilhos[i] += pesosFilhos[i-1];

	double random = randomDouble();
	printf("somatoria: %f\n", somatoriaProb);
	for (i = 0; i < qtaFilhos; i++){
		printf("peso: %f\n", pesosFilhos[i]);
		if (random <= pesosFilhos[i] / somatoriaProb){
			return getFilho(i, filhos);
		}
	}
	return NULL;
}