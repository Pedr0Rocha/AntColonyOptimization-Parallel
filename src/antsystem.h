#ifndef ANTSYSTEM_H
#define ANTSYSTEM_H

#include <stdio.h>


void inicializaFormigas(formiga formigas[], int matrizInicial[4][4], int matrizFinal[4][4]);

void geraNode(int matriz[4][4], node *caminho, int matrizFinal[4][4]);

#endif