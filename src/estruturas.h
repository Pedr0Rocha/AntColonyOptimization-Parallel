#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct node node;
typedef struct listaLigada listaLigada;

struct node {
	int matriz[4][4];
	int valorHeuristica;
	float feromonio;
	int qtaFilhos;
	listaLigada *filhos;
};

struct listaLigada {
	node *nodeAtual;
	listaLigada *prev;
};

typedef struct {
	int id;
	listaLigada *caminho;
	int movimentos;
} formiga;

typedef struct {
	int x, y;
} par;

void inicializaMatrizResposta(int matrizResposta[4][4]);

int calculaQuantidadeFilhos(node *node);

int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]);

void imprimeMatriz(int matriz[4][4]);
void imprimeInfoFormiga(formiga f);

void cloneArray(int matriz[4][4], int clone[4][4]);

void insereListaLigada(node *node, listaLigada **lista);

par achaPosicaoZero(int matriz[4][4]);

#endif