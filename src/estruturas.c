#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas.h"
#include "heuristica.h"

// matriz com a configuracao correta do puzzle
void inicializaMatrizResposta(int matrizResposta[4][4]){
	matrizResposta[0][0] = 1;
	matrizResposta[0][1] = 2;
	matrizResposta[0][2] = 3;
	matrizResposta[0][3] = 4;
	matrizResposta[1][0] = 12;
	matrizResposta[1][1] = 13;
	matrizResposta[1][2] = 14;
	matrizResposta[1][3] = 5;
	matrizResposta[2][0] = 11;
	matrizResposta[2][1] = 0;
	matrizResposta[2][2] = 15;
	matrizResposta[2][3] = 6;
	matrizResposta[3][0] = 10;
	matrizResposta[3][1] = 9;
	matrizResposta[3][2] = 8;
	matrizResposta[3][3] = 7;
}


void cloneArray(int original[4][4], int clone[4][4]){
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			clone[i][j] = original[i][j];
}

// se as matrizes sao iguais retorna 1
int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]){
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (matrizComparar[i][j] != matrizAlvo[i][j]) return 0;
	return 1;
}

// insere no final da lista ligada
void insereListaLigada(node *node, listaLigada **lista) {
	if (*lista == NULL){
		*lista = malloc(sizeof(listaLigada)); 
		(**lista).nodeAtual = node;
		(**lista).prev = NULL;
	} else {
		listaLigada *nova;
		nova = malloc(sizeof(listaLigada));
		nova->nodeAtual = node;
		nova->prev = *lista;
		*lista = nova;
	}
}

// acha a posicao do zero na matriz dada
par achaPosicaoZero(int matriz[4][4]){
	par posZero;
	int i, j;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j)
			if (matriz[i][j] == 0){
				posZero.x = i;
				posZero.y = j;
			}
	return posZero;
}

// calcula quantidade de filhos do node
int calculaQuantidadeFilhos(node *node){
	par posZero;
	int i, j;
	int filhos = 0;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j)
			if (node->matriz[i][j] == 0){
				posZero.x = i;
				posZero.y = j;
			}

	if (posZero.y - 1 >= 0) filhos++;
	if (posZero.y + 1 <  4) filhos++;
	if (posZero.x - 1 >= 0) filhos++;
	if (posZero.x + 1 <  4) filhos++;

	return filhos;
}

void imprimeMatriz(int matriz[4][4]){
	int i, j;
	for (i = 0; i < 4; i++){
		for (j = 0; j < 4; j++){
			if (matriz[i][j] > 9)
				printf("%d ", matriz[i][j]);
			else
				printf("%d  ", matriz[i][j]);
		}
		printf("\n");
	}
}
/*
int rouletteSelect(double[] weight) {
	// calculate the total weight
	double weight_sum = 0;
	for(int i=0; i<weight.length; i++) {
		weight_sum += weight[i];
	}
	// get a random value
	double value = randUniformPositive() * weight_sum;	
	// locate the random value based on the weights
	for(int i=0; i<weight.length; i++) {		
		value -= weight[i];		
		if(value <= 0) return i;
	}
	// only when rounding errors occur
	return weight.length - 1;
}

// Returns a uniformly distributed double value between 0.0 and 1.0
double randUniformPositive() {
	// easiest implementation
	return new Random().nextDouble();
}
*/
node* selecaoRoleta(listaLigada *filhos){
	double somatoriaFeromonio = 0;
	listaLigada *atual = filhos;
	while (atual != NULL){
		somatoriaFeromonio += atual->nodeAtual->feromonio;
		atual = atual->prev;
	}
	printf("somatorio: %f\n", somatoriaFeromonio);
	srand(time(NULL));
	int r = (rand() % (int)somatoriaFeromonio) + 1;
	atual = filhos;
	while (atual != NULL){
		if (atual->nodeAtual->feromonio + r > somatoriaFeromonio)
			return atual->nodeAtual;
		atual = atual->prev;
	}
	return NULL;
}	

void imprimeFilhosNode(node *node){
	listaLigada *atual = node->filhos;
	while (atual != NULL){
		printf("Filho:\n");
		imprimeMatriz(atual->nodeAtual->matriz);
		atual = atual->prev;
		printf("\n");
	}
}