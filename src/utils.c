#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include "estruturas.h"

void inicializaRandom(unsigned long long seed){
	printf("SEED: %llu\n", seed);
	srand(seed);
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
		pesosFilhos[i] = (pow(atual->nodeAtual->feromonio, alfa) * pow((atual->nodeAtual->valorHeuristica+1), beta));
		i++;
		atual = atual->prev;
	}

	double melhorPeso = pesosFilhos[0];
	int index = 0;
	for (i = 1; i < qtaFilhos; i++){
		if (pesosFilhos[i] > melhorPeso){
			melhorPeso = pesosFilhos[i];
			index = i;
		}
	}
	pesosFilhos[index] *= 5;
	for (i = 0; i < qtaFilhos; i++)
		somatoriaProb += pesosFilhos[i];

	double random = randomDouble();

	for (i = 1; i < qtaFilhos; i++)
		pesosFilhos[i] += pesosFilhos[i-1];

	for (i = 0; i < qtaFilhos; i++){
		if (random <= pesosFilhos[i] / somatoriaProb){
			return getFilho(i, filhos);
		}
	}	
	printf("Roleta retornou NULL\n");
	return NULL;
}