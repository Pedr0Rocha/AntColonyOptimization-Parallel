#include <stdio.h>
#include "leitor.h"
#include "estruturas.h"
#include "heuristica.h"
#include "antsystem.h"

#define MAX_CICLOS 1
#define QTA_FORMIGAS 2

void constroiSolucao(formiga formiga){}
void atualizaCustoFormiga(){}
void atualizaFeromonioLocal(){}
void atualizaFeromonioGlobal(){}

void inicializaFormigas(formiga formigas[QTA_FORMIGAS], int matrizInicial[4][4], int matrizFinal[4][4]){
	int i;
	for (i = 0; i < QTA_FORMIGAS; i++){
		formigas[i].id = i;
		cloneArray(matrizInicial, formigas[i].caminho[0].matriz);
		formigas[i].caminho[0].valorHeuristica = calculaHeuristica(matrizFinal, formigas[i].caminho[0].matriz);
		formigas[i].caminho[0].movimentos = 0;
	}
}

int antsystem(int matrizInicial[4][4], int matrizFinal[4][4]){
	formiga formigas[QTA_FORMIGAS];
	int contadorCiclos = 0;
	int i;
	while (contadorCiclos != MAX_CICLOS){
		inicializaFormigas(formigas, matrizInicial, matrizFinal);
		for (i = 0; i < QTA_FORMIGAS; i++){
			imprimeInfoFormiga(formigas[i], 1);
			//constroiSolucao(formigas[i]);
			//atualizaCustoFormiga();
			//if (formigas[i]->custo > melhorCusto)
			//	melhorCusto = formigas[i]->custo;
			//atualizaFeromonioCaminho();
		}
		contadorCiclos++;
		//atualizaFeromonioGlobal();
	}
	return melhorCusto;
}

int main(){
	int matrizInicial[4][4];
	int matrizResposta[4][4];
	inicializaMatrizResposta(matrizResposta);

	printf("\nMatriz Resposta\n");
	imprimeMatriz(matrizResposta);
	printf("\nMatriz Inicial\n");
	leEntrada("entradas/input1.txt", matrizInicial);
	imprimeMatriz(matrizInicial);
	printf("\n\n");

	antsystem(matrizInicial, matrizResposta);

	//printf("Melhor Custo: " + antsystem());
}
