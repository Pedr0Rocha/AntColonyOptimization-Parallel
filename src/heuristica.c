#include <stdio.h>
#include <stdlib.h>
#include "heuristica.h"
#include "estruturas.h"

par mapaPosicoesCorretas[16] = {
	{ 2 , 1 },	{ 0 , 0 },	{ 0 , 1 },	{ 0 , 2 },
	{ 0 , 3 },	{ 1 , 3 },	{ 2 , 3 },	{ 3 , 3 }, 
	{ 3 , 2 }, 	{ 3 , 1 },	{ 3 , 0 },	{ 2 , 0 }, 
	{ 1 , 0 }, 	{ 1 , 1 },	{ 1 , 2 },	{ 2 , 2 }
};

int calculaHeuristica(int matrizResposta[4][4], int matrizComparar[4][4], int heuristica){
	int distanciaX = 0;
	int distanciaY = 0;
	int i, j, resposta; 
	int certas = 16;

	if (heuristica){
		for (i = 0; i < 4; i++){
		    for (j = 0; j < 4; j++){
		        if (matrizComparar[i][j] != matrizResposta[i][j]){
		            distanciaX += abs(mapaPosicoesCorretas[matrizComparar[i][j]].x - i);
		            distanciaY += abs(mapaPosicoesCorretas[matrizComparar[i][j]].y - j);
		        }
		    }
		}
		resposta = 60 - (distanciaX + distanciaY);
	} else {
		for (i = 0; i < 4; i++)
		    for (j = 0; j < 4; j++)
		        if (matrizComparar[i][j] != matrizResposta[i][j])
		        	certas--;   
		resposta = certas;
	}
	return resposta;
}
