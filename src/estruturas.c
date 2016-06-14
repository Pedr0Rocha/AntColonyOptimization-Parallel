#include <stdio.h>
#include "estruturas.h"
#include "heuristica.h"

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


void cloneArray(int original[4][4], int clone[4][4]){
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			clone[i][j] = original[i][j];
}

int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]){
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (matrizComparar[i][j] != matrizAlvo[i][j]) return 0;
	return 1;
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

void imprimeInfoFormiga(formiga f, int imprimeCaminho){
	printf("FORMIGA: %d\n", f.id);
	printf("Valor da Heuristica: %d\n", f.caminho[sizeof(f.caminho)].valorHeuristica);
	printf("Matriz Final:\n");
	imprimeMatriz(f.caminho[sizeof(f.caminho)].matriz);
	printf("\n\n");
	if (imprimeCaminho == 1){
		printf("Caminho:");
		int i;
		for (i = 0; i < sizeof(f.caminho); i++){
			imprimeMatriz(f.caminho[i].matriz);
			printf("\n");
		}
	}
}