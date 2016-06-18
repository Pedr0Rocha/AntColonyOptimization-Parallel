#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"
#include "heuristica.h"

// matriz com a configuracao correta do puzzle
void inicializaMatrizResposta(int matrizResposta[4][4]){
	/*
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
	*/
	matrizResposta[0][0] = 1;
	matrizResposta[0][1] = 2;
	matrizResposta[0][2] = 3;
	matrizResposta[0][3] = 4;
	matrizResposta[1][0] = 5;
	matrizResposta[1][1] = 6;
	matrizResposta[1][2] = 7;
	matrizResposta[1][3] = 8;
	matrizResposta[2][0] = 9;
	matrizResposta[2][1] = 10;
	matrizResposta[2][2] = 11;
	matrizResposta[2][3] = 12;
	matrizResposta[3][0] = 13;
	matrizResposta[3][1] = 14;
	matrizResposta[3][2] = 15;
	matrizResposta[3][3] = 0;
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

// retorna o filho com index i
node* getFilho(int i, listaLigada *filhos){
	if (i == 0) return filhos->nodeAtual;
	else {
		int contador = 0;
		listaLigada *atual = filhos;
		while (i != contador){
			contador++;
			atual = atual->prev;
		}
		return atual->nodeAtual;
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

// confere se a matriz ja esta no caminho da formiga
int estaNoCaminho(int matriz[4][4], formiga *formiga){
	listaLigada *atual = formiga->caminho;
	while (atual != NULL){
		if (matrizIgual(matriz, atual->nodeAtual->matriz) == 1) return 1;
		atual = atual->prev;
	}
	return 0;
}

void voltaRaizCaminho(formiga *formiga, listaLigada **caminho){
	listaLigada *atual = formiga->caminho;
	while (atual != NULL){
		atual = atual->prev;
	}
	imprimeMatriz(atual->nodeAtual->matriz);
	//(**caminho).nodeAtual = atual->nodeAtual;
	//(**caminho).prev = NULL;
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
			if (matriz[i][j] == 0)
				printf("\033[31;1m%d  \033[0m", matriz[i][j]);
			else if (matriz[i][j] > 9)
				printf("%d ", matriz[i][j]);
			else
				printf("%d  ", matriz[i][j]);
		}
		printf("\n");
	}
}	

void imprimeFilhosNode(node *node){
	listaLigada *atual = node->filhos;
	printf("Qta filhos: %d\n", node->qtaFilhos);
	while (atual != NULL){
		printf("Filho:\n");
		imprimeMatriz(atual->nodeAtual->matriz);
		atual = atual->prev;
		printf("\n");
	}
}

void imprimeNode(node *node){
	printf("Node\n");
	imprimeMatriz(node->matriz);
	printf("\nValor Heuristica: %d\n", node->valorHeuristica);
	printf("Feromonio: %f\n", node->feromonio);
	imprimeFilhosNode(node);
	printf("\n\n");
}

void imprimeCaminhoFormiga(formiga *formiga){
	listaLigada *atual = formiga->caminho;
	printf("Caminho da Formiga\n");
	while (atual != NULL){
		imprimeMatriz(atual->nodeAtual->matriz);
		atual = atual->prev;
		printf("\n");
	}
}