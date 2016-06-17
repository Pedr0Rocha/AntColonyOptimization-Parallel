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

// acha a posicao do zero na matriz dada
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