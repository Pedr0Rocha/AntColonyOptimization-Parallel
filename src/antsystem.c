#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "estruturas.h"
#include "heuristica.h"
#include "antsystem.h"

#define MAX_CICLOS 1
#define QTA_FORMIGAS 2

void atualizaFeromonioCaminho(){}
void atualizaFeromonioGlobal(){}

// TODO - checar probabilidades e inserir apenas um node no caminho
void geraNode(int matriz[4][4], node *caminho, int matrizFinal[4][4]){
	par zeroPos;
	zeroPos = achaPosicaoZero(matriz);
	int movimentos = caminho->movimentos + 1;

	// vizinho na coluna da esquerda
	if (zeroPos.y - 1 >= 0){
		node nodeEsquerda;

		cloneArray(matriz, nodeEsquerda.matriz);
		// troca posição do zero na matriz nova
		nodeEsquerda.matriz[zeroPos.x][zeroPos.y] = nodeEsquerda.matriz[zeroPos.x][zeroPos.y - 1];
		nodeEsquerda.matriz[zeroPos.x][zeroPos.y - 1] = 0;
		nodeEsquerda.valorHeuristica = calculaHeuristica(matrizFinal, nodeEsquerda.matriz, 1);
		insereNoCaminho(caminho, nodeEsquerda, movimentos);
	}
	// vizinho na coluna da direita
	if (zeroPos.y + 1 < 4){
		node nodeDireita;

		cloneArray(matriz, nodeDireita.matriz);
		// troca posição do zero na matriz nova
		nodeDireita.matriz[zeroPos.x][zeroPos.y] = nodeDireita.matriz[zeroPos.x][zeroPos.y + 1];
		nodeDireita.matriz[zeroPos.x][zeroPos.y + 1] = 0;
		nodeDireita.valorHeuristica = calculaHeuristica(matrizFinal, nodeDireita.matriz, 1);
		insereNoCaminho(caminho, nodeDireita, movimentos);
	}
	// vizinho na linha de cima
	if (zeroPos.x - 1 >= 0){
		node nodeCima;

		cloneArray(matriz, nodeCima.matriz);
		// troca posição do zero na matriz nova
		nodeCima.matriz[zeroPos.x][zeroPos.y] = nodeCima.matriz[zeroPos.x - 1][zeroPos.y];
		nodeCima.matriz[zeroPos.x - 1][zeroPos.y] = 0;
		nodeCima.valorHeuristica = calculaHeuristica(matrizFinal, nodeCima.matriz, 1);
		insereNoCaminho(caminho, nodeCima, movimentos);
	}
	// vizinho na linha de baixo
	if (zeroPos.x + 1 < 4){
		node nodeBaixo;

		cloneArray(matriz, nodeBaixo.matriz);
		// troca posição do zero na matriz nova
		nodeBaixo.matriz[zeroPos.x][zeroPos.y] = nodeBaixo.matriz[zeroPos.x + 1][zeroPos.y];
		nodeBaixo.matriz[zeroPos.x + 1][zeroPos.y] = 0;
		nodeBaixo.valorHeuristica = calculaHeuristica(matrizFinal, nodeBaixo.matriz, 1);
		insereNoCaminho(caminho, nodeBaixo, movimentos);
	}
}

void inicializaFormigas(formiga formigas[QTA_FORMIGAS], int matrizInicial[4][4], int matrizFinal[4][4]){
	int i;
	for (i = 0; i < QTA_FORMIGAS; i++){
		formigas[i].id = i;
		formigas[i].caminho = malloc(sizeof(node));
		cloneArray(matrizInicial, formigas[i].caminho->matriz);
		formigas[i].caminho->valorHeuristica = calculaHeuristica(matrizFinal, formigas[i].caminho->matriz, 1);
		formigas[i].caminho->movimentos = 0;
		formigas[i].caminho->prox = NULL;
	}
}

int antsystem(int matrizInicial[4][4], int matrizFinal[4][4]){
	formiga formigas[QTA_FORMIGAS];
	int contadorCiclos = 0;
	int i;
	while (contadorCiclos != MAX_CICLOS){
		inicializaFormigas(formigas, matrizInicial, matrizFinal);
		for (i = 0; i < QTA_FORMIGAS; i++){
			geraNode(formigas[i].caminho->matriz, formigas[i].caminho, matrizFinal);
			imprimeInfoFormiga(formigas[i]);
			//if (formigas[i]->custo > melhorCusto)
			//	melhorCusto = formigas[i]->custo;
			//atualizaFeromonioCaminho();
		}
		contadorCiclos++;
		//atualizaFeromonioGlobal();
	}
	return 0;
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

/*
Adiciona node no caminho da formiga
formigas[i].caminho->prox = malloc(sizeof(node));
cloneArray(matrizInicial, formigas[i].caminho->prox->matriz);
formigas[i].caminho->prox->valorHeuristica = calculaHeuristica(matrizFinal, formigas[i].caminho->prox->matriz, 1);
formigas[i].caminho->prox->movimentos = 1;
formigas[i].caminho->prox->prox = NULL;
*/
