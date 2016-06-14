#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct {
	int matrizAtual[4][4];
	int valor_heuristica;
	int movimentos;
} node;

typedef struct {
	node solucao;
	int custo;
} formiga;

void inicializaMatrizResposta(int matrizResposta[4][4]);
//void inicializaFormigas(int qta_formigas);

void imprimeMatriz(int matriz[4][4]);

void expandeNodo(node node);

#endif