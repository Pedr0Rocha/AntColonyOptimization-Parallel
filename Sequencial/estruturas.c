#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "includes/estruturas.h"
#include "includes/heuristica.h"

void cloneArray(int original[4][4], int clone[4][4]){
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			clone[i][j] = original[i][j];
}

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

int queueVazio(listaLigada *lista) {
	int count = 0;
	while (lista != NULL) {
		count++;
		lista = lista->prev;
	}
	if (count == 0) return 1;
	else return 0;
}

void removeListaLigada(node *nodeRem, listaLigada **lista) {
	if (matrizIgual(nodeRem->matriz, (**lista).nodeAtual->matriz)) {
		node *temp = *lista;
		*lista = (**lista).prev;
		free(temp);
		return;
	}

	node *atual = (**lista)->prev;
	node *prev = *lista;
	while (atual != NULL && prev != NULL) {
		if (matrizIgual(nodeRem->matriz, atual->matriz)) {
			node *temp = atual;
			prev->prev = atual->prev;
			free(temp);
			return;
		}
		prev = atual;	
		atual = prev->prev;
	}
    return;
}

node* getFilho(int index, listaLigada *filhos){
	if (index == 0) return filhos->nodeAtual;
	else {
		int contador = 0;
		listaLigada *atual = filhos;
		while (index != contador){
			contador++;
			atual = atual->prev;
		}
		return atual->nodeAtual;
	}
}

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

// checa se todos os filhos ja estao no caminho
int todosNoCaminho(formiga *formiga){
	listaLigada *atual = formiga->caminho.todos->nodeAtual->filhos;
	while (atual != NULL){
		if (!getNoCaminhoExiste(atual->nodeAtual->matriz, &formiga->caminho)) return 0;
		atual = atual->prev;
	}
	return 1;
}

// confere se a matriz ja esta na arvore
node* getNoCaminhoExiste(int matriz[4][4], hashmap *hash) {
	return buscaHash(matriz, hash);
}

int calculaQuantidadeFilhos(node *node){
	int filhos = 0;
	par posZero = achaPosicaoZero(node->matriz);
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

node* buscaHash(int matriz[4][4], hashmap *hash) {
	int key = geraHashKey(matriz, hash->qtaBuckets);
	listaLigada *atual = hash->buckets[key];
	while (atual != NULL){
		if (matrizIgual(atual->nodeAtual->matriz, matriz))
			return atual->nodeAtual;
		atual = atual->prev;
	}
	return NULL;
}

int geraHashKey(int matriz[4][4], int qtaBuckets){
	int i, j;
	int key = 0;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			key = (matriz[i][j] + (key * (65599 % qtaBuckets) % qtaBuckets)) % qtaBuckets;
	return key;
}

void insereHash(node *node, hashmap *hash) {
	int key = geraHashKey(node->matriz, hash->qtaBuckets);
	insereListaLigada(node, &hash->buckets[key]);
	insereListaLigada(node, &hash->todos);
	hash->qtaNodes++;
}

void inicializaHash(hashmap *hash, int qtaBuckets){
	hash->qtaNodes = 0;
	hash->qtaBuckets = qtaBuckets;
	hash->todos = NULL;
	int i;
	hash->buckets = malloc(sizeof(listaLigada *) * qtaBuckets);
	for (i = 0; i < qtaBuckets; i++)
		hash->buckets[i] = NULL;
}
