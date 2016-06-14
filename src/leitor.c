#include <stdio.h>
#include "leitor.h"


void leEntrada(char *caminho, int matriz[4][4]){
	FILE *fp;

	fp = fopen(caminho, "r");
	if (fp != NULL){
		int i, j;
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				fscanf(fp, "%d", &matriz[i][j]);
	} else {
		printf("Erro ao ler arquivo de entrada.");
	}
}