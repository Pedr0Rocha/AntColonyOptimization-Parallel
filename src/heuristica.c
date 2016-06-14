#include <stdio.h>
#include <stdlib.h>
#include "heuristica.h"
#include "estruturas.h"

int calculaHeuristica(int matrizResposta[4][4], int matrizComparar[4][4]){
	int distanciaX = 0;
	int distanciaY = 0;
	int i, j; 
	par posicaoCorreta[16];

	posicaoCorreta[0]  = new par(2, 1);
	posicaoCorreta[1]  = new par(0, 0);
	posicaoCorreta[2]  = new par(0, 1);
	posicaoCorreta[3]  = new par(0, 2);
	posicaoCorreta[4]  = new par(0, 3);
	posicaoCorreta[5]  = new par(1, 3);
	posicaoCorreta[6]  = new par(2, 3);
	posicaoCorreta[7]  = new par(3, 3);
	posicaoCorreta[7]  = new par(3, 2);
	posicaoCorreta[8]  = new par(3, 1);
	posicaoCorreta[9]  = new par(3, 0);
	posicaoCorreta[10] = new par(2, 0);
	posicaoCorreta[11] = new par(1, 0);
	posicaoCorreta[12] = new par(1, 1);
	posicaoCorreta[13] = new par(1, 2);
	posicaoCorreta[14] = new par(2, 2);
	 
	int erradas = 0;

	for (i = 0; i < 4; i++){
	    for (j = 0; j < 4; j++){
	        if (matrizComparar[i][j] != matrizResposta[i][j]){
	        	erradas++;
	            distanciaX += abs(posicaoCorreta[matrizComparar[i][j]].x - i);
	            distanciaY += abs(posicaoCorreta[matrizComparar[i][j]].y - j);
	        }
	    }
	}
	printf("Distancia X = %d, Distancia Y = %d", distanciaX, distanciaY);
	printf("Pecas fora do lugar: %d", erradas);
	return (distanciaX + distanciaY);
}