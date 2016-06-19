#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "utils.h"
#include "estruturas.h"

void inicializaRandom(int seed){
	//srand(seed);
	unsigned long long s = time(NULL);
	printf("SEED: %llu\n", s);
	srand(s);
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
		//printf("Feromonio %f, Heuristica: %d\n", atual->nodeAtual->feromonio, atual->nodeAtual->valorHeuristica);
		pesosFilhos[i] = (pow(atual->nodeAtual->feromonio, alfa) * pow((atual->nodeAtual->valorHeuristica+1), beta));
		somatoriaProb += pesosFilhos[i];
		//printf("pesosFilhos %d: %f\n", i, pesosFilhos[i]);
		i++;
		atual = atual->prev;
	}
	//printf("i = %d\n", i);
	double random = randomDouble();
	//printf("Quantidade de filhos: %d\n", qtaFilhos);
	//printf("Somatoria: %f\n", somatoriaProb);
	//for (i = 0; i < qtaFilhos; i++){
		//printf("Pesos do filho %d: %f\n",i, pesosFilhos[i]);
		//printf("random %f <= %f\n\n", random, (pesosFilhos[i] / somatoriaProb));
	//}
	for (i = 1; i < qtaFilhos; i++)
		pesosFilhos[i] += pesosFilhos[i-1];

	for (i = 0; i < qtaFilhos; i++){
		//printf("random %f <= %f\n", random, (pesosFilhos[i] / somatoriaProb));
		if (random <= pesosFilhos[i] / somatoriaProb){
			return getFilho(i, filhos);
		}
	}
	//printf("FUDEU\n");
	return NULL;
}