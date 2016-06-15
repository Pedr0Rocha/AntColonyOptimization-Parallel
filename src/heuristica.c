#include <stdio.h>
#include <stdlib.h>
#include "heuristica.h"
#include "estruturas.h"

par mapaPosicoes[16];

int calculaHeuristica(int matrizResposta[4][4], int matrizComparar[4][4], int flag){
	fazMapa();
	int distanciaX = 0;
	int distanciaY = 0;
	int i, j; 
	int erradas = 0;

	for (i = 0; i < 4; i++){
	    for (j = 0; j < 4; j++){
	        if (matrizComparar[i][j] != matrizResposta[i][j]){
	        	erradas++;
	            distanciaX += abs(mapaPosicoes[matrizComparar[i][j]].x - i);
	            distanciaY += abs(mapaPosicoes[matrizComparar[i][j]].y - j);
	        }
	    }
	}
	int resposta;
	if (flag == 1) 
		resposta = (distanciaX + distanciaY);
	else 
		resposta = erradas;

	return resposta;
}

void fazMapa(){
	mapaPosicoes[0].x = 2; 
	mapaPosicoes[0].y = 1;

	mapaPosicoes[1].x = 0;  
	mapaPosicoes[1].y = 0;
	
	mapaPosicoes[2].x = 0;  
	mapaPosicoes[2].y = 1;
	
	mapaPosicoes[3].x = 0;  
	mapaPosicoes[3].y = 2;
	
	mapaPosicoes[4].x = 0;  
	mapaPosicoes[4].y = 3;
	
	mapaPosicoes[5].x = 1;  
	mapaPosicoes[5].y = 3;
	
	mapaPosicoes[6].x = 2;  
	mapaPosicoes[6].y = 3;
	
	mapaPosicoes[7].x = 3;  
	mapaPosicoes[7].y = 3;
	
	mapaPosicoes[8].x = 3;  
	mapaPosicoes[8].y = 2;
	
	mapaPosicoes[9].x = 3;  
	mapaPosicoes[9].y = 1;
	
	mapaPosicoes[10].x = 3;  
	mapaPosicoes[10].y = 0;
	
	mapaPosicoes[11].x = 2; 
	mapaPosicoes[11].y = 0;
	
	mapaPosicoes[12].x = 1; 
	mapaPosicoes[12].y = 0;
	
	mapaPosicoes[13].x = 1; 
	mapaPosicoes[13].y = 1;
	
	mapaPosicoes[14].x = 1; 
	mapaPosicoes[14].y = 2;
	
	mapaPosicoes[15].x = 2; 
	mapaPosicoes[15].y = 2;
}