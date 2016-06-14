#include <stdio.h>
#include "leitor.h"
#include "estruturas.h"
#include "heuristica.h"
#include "antsystem.h"

#define MAX_CICLOS 5;
#define QTA_FORMIGAS 5;

// void constroiSolucao(formiga formiga){}
// void atualizaCustoFormiga(){}
// void atualizaFeromonioLocal(){}
// void atualizaFeromonioGlobal(){}

// int antsystem(){
// 	int contador_ciclos = 0;
// 	int i;
// 	int melhorCusto = 999999;
// 	while (contador_ciclos != MAX_CICLOS){
// 		formigas = inicializaFormigas(QTA_FORMIGAS);
// 		for (i = 0; i < QTA_FORMIGAS; i++){
// 			constroiSolucao(formigas[i]);
// 			atualizaCustoFormiga();
// 			if (formigas[i]->custo > melhorCusto)
// 				melhorCusto = formigas[i]->custo;
// 			atualizaFeromonioLocal();
// 		}
// 		atualizaFeromonioGlobal();
// 	}
// 	return melhorCusto;
// }

int main(){
	int matrizInicial[4][4];
	int matrizResposta[4][4];
	inicializaMatrizResposta(matrizResposta);

	printf("Matriz Resposta\n");
	imprimeMatriz(matrizResposta);
	printf("Matriz Inicial\n");
	leEntrada("entradas/input1.txt", matrizInicial);
	imprimeMatriz(matrizInicial);

	//printf("Melhor Custo: " + antsystem());
}
