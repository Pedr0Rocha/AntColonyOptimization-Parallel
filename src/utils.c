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
		//printf("pesosFilhos %d: %f\n", i, pesosFilhos[i]);
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
	//printf("i = %d\n", i);
	double random = randomDouble();
	//printf("Quantidade de filhos: %d\n", qtaFilhos);
	//printf("Somatoria: %f\n", somatoriaProb);
	//for (i = 0; i < qtaFilhos; i++){
	//	printf("Pesos do filho %d: %f\n",i, pesosFilhos[i]);
	//	printf("random %f <= %f\n\n", random, (pesosFilhos[i] / somatoriaProb));
	//} 

	for (i = 1; i < qtaFilhos; i++)
		pesosFilhos[i] += pesosFilhos[i-1];

	for (i = 0; i < qtaFilhos; i++){
		//printf("random %f <= %f\n", random, (pesosFilhos[i] / somatoriaProb));
		if (random <= pesosFilhos[i] / somatoriaProb){
			return getFilho(i, filhos);
		}
	}	
	// printf("i = %d\n", i);
	// printf("Quantidade de filhos: %d\n", qtaFilhos);	
	// atual = filhos;
	// while (atual != NULL){
	// 	printf("Feromonio %f, Heuristica: %d\n", atual->nodeAtual->feromonio, atual->nodeAtual->valorHeuristica);
	// 	atual = atual->prev;
	// }
	// printf("Somatoria: %f\n", somatoriaProb);
	// for (i = 0; i < qtaFilhos; i++){
	// 	printf("Pesos do filho %d: %f\n",i, pesosFilhos[i]);
	// 	printf("random %f <= %f\n\n", random, (pesosFilhos[i] / somatoriaProb));
	// }
	// printf("FUDEU\n");
	return NULL;
}