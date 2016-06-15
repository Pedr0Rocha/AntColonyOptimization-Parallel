#include <stdio.h>
#include <stdlib.h>
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

// insere node no final do caminho
void insereNoCaminho(node *caminho, node nodeInserir, int movimentos){
	node *atual = caminho;
	while (atual->prox != NULL)
		atual = atual->prox;

	atual->prox = malloc(sizeof(node));
	cloneArray(nodeInserir.matriz, atual->prox->matriz);
	atual->prox->valorHeuristica = nodeInserir.valorHeuristica;
	atual->prox->movimentos = movimentos;
	atual->prox->prox = NULL;
}

// retorna o numero de movimentos do ultimo node do caminho
int numeroMovimentosSolucao(node *caminho){
	node *atual = caminho;
	while (atual != NULL)
		atual = atual->prox;

	return atual->movimentos;
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

void imprimeInfoFormiga(formiga f){
	printf("FORMIGA: %d\n", f.id);
	printf("\n");
	printf("Caminho:\n");
	node *atual = f.caminho;
	while (atual != NULL){
		imprimeMatriz(atual->matriz);
		printf("\n");
		printf("Valor da Heuristica: %d\n", atual->valorHeuristica);
		printf("Movimentos: %d\n", atual->movimentos);
		printf("--------------------------------\n\n");
		atual = atual->prox;
	}
}