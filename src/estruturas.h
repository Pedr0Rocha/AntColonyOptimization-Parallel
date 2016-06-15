#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
typedef struct node node;

struct node{
	int matriz[4][4];
	int valorHeuristica;
	int movimentos;
	struct node *prox;
};

typedef struct {
	int id;
	node *caminho;
	float feromonio;
	float probCaminho;
} formiga;

typedef struct {
	int x, y;
} par;

void inicializaMatrizResposta(int matrizResposta[4][4]);

int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]);

void imprimeMatriz(int matriz[4][4]);
void imprimeInfoFormiga(formiga f);

void cloneArray(int matriz[4][4], int clone[4][4]);

void insereNoCaminho(node *caminho, node nodeInserir, int movimentos);

par achaPosicaoZero(int matriz[4][4]);

#endif