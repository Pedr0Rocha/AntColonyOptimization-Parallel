#include <stdio.h>
#include "estruturas.h"

void inicializaMatrizResposta(int matrizResposta[4][4]){
	matrizResposta[0][0] = 1;
	matrizResposta[0][1] = 2;
	matrizResposta[0][2] = 3;
	matrizResposta[0][3] = 4;
	matrizResposta[1][0] = 12;
	matrizResposta[1][1] = 13;
	matrizResposta[1][2] = 14;
	matrizResposta[1][3] = 5;
	matrizResposta[2][0] = 11;
	matrizResposta[2][1] = 0;
	matrizResposta[2][2] = 15;
	matrizResposta[2][3] = 6;
	matrizResposta[3][0] = 10;
	matrizResposta[3][1] = 9;
	matrizResposta[3][2] = 8;
	matrizResposta[3][3] = 7;
}

// formiga[qta_formigas] inicializaFormigas(int qta_formigas){
// 	int i;
// 	formiga formigas[qta_formigas];
// 	for (i = 0; i < qta_formigas; i++){
// 	}
// 	return formigas;
// }

void expandeNodo(node nodo){
	
}

void imprimeMatriz(int matriz[4][4]){
	int i, j;
	for (i = 0; i < 4; i++){
		for (j = 0; j < 4; j++){
			if (matriz[i][j] > 9)
				printf("%d ", matriz[i][j]);
			else
				printf("%d  ", matriz[i][j]);
		}
		printf("\n");
	}
}