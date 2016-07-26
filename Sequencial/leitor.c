#include <stdio.h>
#include "includes/leitor.h"


void leEntrada(char *caminho, int matriz[4][4]){
	FILE *fp;

	int bytes = 0;

	fp = fopen(caminho, "r");
	if (fp != NULL){
		int i, j;
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				bytes = fscanf(fp, "%d", &matriz[i][j]);
	} else {
		printf("Erro ao ler arquivo de entrada. Bytes = %d", bytes);
	}
}