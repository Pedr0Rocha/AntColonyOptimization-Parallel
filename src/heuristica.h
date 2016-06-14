#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <stdio.h>


//	HEURISTICA DE DISTANCIA MANHATTAN
//	Para cada peça fora do lugar, somar a distancia retangular para colocar no lugar correto.
int calculaHeuristica(int matrizResposta[4][4], int matrizComparar[4][4]);

void fazMapa();

#endif