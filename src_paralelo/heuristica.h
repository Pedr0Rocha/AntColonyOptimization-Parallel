#ifndef HEURISTICA_H
#define HEURISTICA_H

#include <stdio.h>


//	heuristica distancia manhattan ou peças fora do lugar, 0 para peças fora do lugar, 1 para manhattan
int calculaHeuristica(int matrizResposta[4][4], int matrizComparar[4][4], int heuristica);
void fazMapa();

#endif