#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct {
	int matriz[4][4];
	int valorHeuristica;
	int movimentos;
	float feromonio;
} node;

typedef struct {
	int id;
	node caminho[100];
	float probCaminho;
} formiga;

typedef struct {
	int x, y;
} par;

void inicializaMatrizResposta(int matrizResposta[4][4]);

int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]);

void imprimeMatriz(int matriz[4][4]);
void imprimeInfoFormiga(formiga f, int imprimeCaminho);

void cloneArray(int matriz[4][4], int clone[4][4]);

#endif