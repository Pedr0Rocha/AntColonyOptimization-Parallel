#include <stdio.h>
#include <stdlib.h>
#include "leitor.h"
#include "estruturas.h"
#include "heuristica.h"
#include "antsystem.h"

#define MAX_CICLOS 1
#define QTA_FORMIGAS 2

int matrizInicial[4][4];
int matrizResposta[4][4];

// heuristica usada, 1 - manhattan distance, 0 - out of order
int heuristicaUsada = 1;

/*
	modelagem das equações

	equacao de probabilidade
	prob do caminho = [feromonio pra depositar]^alfa   *  [(1/valorHeuristica + 0.01)]^beta 
					   ---------------------------------------------------------------------
					   Somatório([feromonio pra depositar]^alfa   *  [(1/valorHeuristica + 0.01)]^beta)

	feromonio pra depositar é a quantidade de feromonio que cada formiga depositará nos nodes 
	do caminho encontrado. este valor é alterado depois da formiga terminar de achar a solução. valor = 1 (primeira iteração)

	para calcular o novo feromonio a depositar na trilha usa-se a formula
	fer = fer * (1 - rho) + delta

	rho é a taxa de evaporação. valor = 0.5
	
	delta = (10 / numero de movimentos para chegar na soluçao)
	quanto menor o numero de movimentos, maior o delta e mais feromonio sera depositado naquele caminho

	alfa é o peso para o feromonio aplicado nos nodes. valor = 0.10

	valor da heuristica é o valor retornado pela heuristica usada. Manhattan distance
	usa 1/valorHeuristica + 0.1 e out of order usa apenas o valor. Quanto maior o valor, melhor.
	
	beta é o peso dado para o valor da heuristica. valor = 1



*/



void atualizaFeromonioCaminho(){}
void atualizaFeromonioGlobal(){}

// TODO - checar probabilidades e inserir apenas um node no caminho
void geraNode(int matriz[4][4], node *caminho){
	par zeroPos;
	zeroPos = achaPosicaoZero(matriz);	
	//int movimentos = caminho->movimentos + 1;

	// vizinho na coluna da esquerda
	if (zeroPos.y - 1 >= 0){
		node nodeEsquerda;

		cloneArray(matriz, nodeEsquerda.matriz);
		// troca posição do zero na matriz nova
		nodeEsquerda.matriz[zeroPos.x][zeroPos.y] = nodeEsquerda.matriz[zeroPos.x][zeroPos.y - 1];
		nodeEsquerda.matriz[zeroPos.x][zeroPos.y - 1] = 0;
		nodeEsquerda.valorHeuristica = calculaHeuristica(matrizResposta, nodeEsquerda.matriz, heuristicaUsada);
		//insereNoCaminho(caminho, nodeEsquerda, movimentos);
	}
	// vizinho na coluna da direita
	if (zeroPos.y + 1 < 4){
		node nodeDireita;

		cloneArray(matriz, nodeDireita.matriz);
		// troca posição do zero na matriz nova
		nodeDireita.matriz[zeroPos.x][zeroPos.y] = nodeDireita.matriz[zeroPos.x][zeroPos.y + 1];
		nodeDireita.matriz[zeroPos.x][zeroPos.y + 1] = 0;
		nodeDireita.valorHeuristica = calculaHeuristica(matrizResposta, nodeDireita.matriz, heuristicaUsada);
		//insereNoCaminho(caminho, nodeDireita, movimentos);
	}
	// vizinho na linha de cima
	if (zeroPos.x - 1 >= 0){
		node nodeCima;

		cloneArray(matriz, nodeCima.matriz);
		// troca posição do zero na matriz nova
		nodeCima.matriz[zeroPos.x][zeroPos.y] = nodeCima.matriz[zeroPos.x - 1][zeroPos.y];
		nodeCima.matriz[zeroPos.x - 1][zeroPos.y] = 0;
		nodeCima.valorHeuristica = calculaHeuristica(matrizResposta, nodeCima.matriz, heuristicaUsada);
		//insereNoCaminho(caminho, nodeCima, movimentos);
	}
	// vizinho na linha de baixo
	if (zeroPos.x + 1 < 4){
		node nodeBaixo;

		cloneArray(matriz, nodeBaixo.matriz);
		// troca posição do zero na matriz nova
		nodeBaixo.matriz[zeroPos.x][zeroPos.y] = nodeBaixo.matriz[zeroPos.x + 1][zeroPos.y];
		nodeBaixo.matriz[zeroPos.x + 1][zeroPos.y] = 0;
		nodeBaixo.valorHeuristica = calculaHeuristica(matrizResposta, nodeBaixo.matriz, heuristicaUsada);
		//insereNoCaminho(caminho, nodeBaixo, movimentos);
	}
}

void inicializaFormigas(formiga formigas[QTA_FORMIGAS], node *raiz){
	int i;
	for (i = 0; i < QTA_FORMIGAS; i++){
		formigas[i].id = i;
		formigas[i].caminho = raiz;
	}
}

void inicializaArvore(node *raiz){
	cloneArray(matrizInicial, raiz->matriz);
	raiz->valorHeuristica = calculaHeuristica(matrizResposta, raiz->matriz, heuristicaUsada);
	raiz->movimentos = 0;
	raiz->feromonio = -1;
	raiz->qtaFilhos = calculaQuantidadeFilhos(raiz);
	raiz->filhos = malloc(sizeof(node) * raiz->qtaFilhos);
	raiz->pai = NULL;
}

int antsystem(){
	formiga formigas[QTA_FORMIGAS];
	int i, contadorCiclos = 0;
	node raizArvore;
	inicializaArvore(&raizArvore);
	while (contadorCiclos != MAX_CICLOS){
		inicializaFormigas(formigas, &raizArvore);
		for (i = 0; i < QTA_FORMIGAS; i++){
			//geraNode(formigas[i].caminho->matriz, formigas[i].caminho);
			//imprimeInfoFormiga(formigas[i]);	
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
	inicializaMatrizResposta(matrizResposta);

	printf("\nMatriz Resposta\n");
	imprimeMatriz(matrizResposta);
	printf("\nMatriz Inicial\n");
	leEntrada("entradas/input1.txt", matrizInicial);
	imprimeMatriz(matrizInicial);
	printf("\n\n");

	antsystem();

	//printf("Melhor Custo: " + antsystem());
}