#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct node node;

struct node{
	int matriz[4][4];
	int valorHeuristica;
	int movimentos;
	float feromonio;
	int qtaFilhos;
	node *filhos;
	node *pai;
};

typedef struct {
	int id;
	node *caminho;
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

void insereNoCaminho(node *caminho, node nodeInserir, int movimentos);

par achaPosicaoZero(int matriz[4][4]);

#endif