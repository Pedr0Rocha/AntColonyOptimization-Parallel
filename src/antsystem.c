#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "leitor.h"
#include "utils.h"
#include "estruturas.h"
#include "heuristica.h"

#define MAX_CICLOS 3
#define QTA_FORMIGAS 10
#define ALTURA_ARVORE_MAX 10000

int matrizInicial[4][4];
int matrizResposta[4][4];

node raizArvore;
listaLigada *nodesInseridosArvore = NULL;

// heuristica usada, 1 - manhattan distance, 0 - order
int heuristicaUsada = 0;

// peso aplicado no feromonio
double alfa = 0.10;
// peso aplicado na heuristica
double beta = 1;
// taxa de evapocarao
double rho = 0.5;



void atualizaFeromonioCaminho(formiga *formiga){
	double delta;
	delta = 10 / formiga->movimentos;
	listaLigada *atual = formiga->caminho;
	double limiteInferior = 0.01;
	while (atual != NULL){
		if (formiga->resolvido)
			atual->nodeAtual->feromonio = atual->nodeAtual->feromonio * (1 - rho) + delta;
		else 
			atual->nodeAtual->feromonio *= 0.7;

		if (atual->nodeAtual->feromonio < limiteInferior) 
			atual->nodeAtual->feromonio = limiteInferior;
		atual = atual->prev;
	}
}

void atualizaFeromonioGlobal(){
	listaLigada *atual = nodesInseridosArvore;
	double limiteInferior = 0.01;
	while (atual != NULL){
		nodesInseridosArvore->nodeAtual->feromonio -= rho;
		if (nodesInseridosArvore->nodeAtual->feromonio <= 0) 
			nodesInseridosArvore->nodeAtual->feromonio = limiteInferior;
		atual = atual->prev;
	}
}

void inicializaFilho(node *filho){
	filho->valorHeuristica = calculaHeuristica(matrizResposta, filho->matriz, heuristicaUsada);
	filho->feromonio = 1;
	filho->filhos = NULL;
}

void geraNode(node *nodeOrigem) {
	par zeroPos;
	zeroPos = achaPosicaoZero(nodeOrigem->matriz);	
	int qtaFilhos = 0;
	node *ptNode;
	// vizinho na coluna da esquerda
	//printf("gerandoNode\n");
	if (zeroPos.y - 1 >= 0) {
		int matrizTemp[4][4];
		cloneArray(nodeOrigem->matriz, matrizTemp);
		matrizTemp[zeroPos.x][zeroPos.y] = matrizTemp[zeroPos.x][zeroPos.y - 1];
		matrizTemp[zeroPos.x][zeroPos.y - 1] = 0;
		qtaFilhos++;
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, nodesInseridosArvore))){
			node *filhoEsquerda = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoEsquerda->matriz);
			filhoEsquerda->matriz[zeroPos.x][zeroPos.y] = filhoEsquerda->matriz[zeroPos.x][zeroPos.y - 1];
			filhoEsquerda->matriz[zeroPos.x][zeroPos.y - 1] = 0;
			inicializaFilho(filhoEsquerda);
			insereListaLigada(filhoEsquerda, &nodeOrigem->filhos);
			insereListaLigada(filhoEsquerda, &nodesInseridosArvore);
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
	// vizinho na coluna da direita
	if (zeroPos.y + 1 < 4) {
		int matrizTemp[4][4];
		cloneArray(nodeOrigem->matriz, matrizTemp);
		matrizTemp[zeroPos.x][zeroPos.y] = matrizTemp[zeroPos.x][zeroPos.y + 1];
		matrizTemp[zeroPos.x][zeroPos.y + 1] = 0;
		qtaFilhos++;
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, nodesInseridosArvore))){
			node *filhoDireita = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoDireita->matriz);
			filhoDireita->matriz[zeroPos.x][zeroPos.y] = filhoDireita->matriz[zeroPos.x][zeroPos.y + 1];
			filhoDireita->matriz[zeroPos.x][zeroPos.y + 1] = 0;
			inicializaFilho(filhoDireita);
			insereListaLigada(filhoDireita, &nodeOrigem->filhos);
			insereListaLigada(filhoDireita, &nodesInseridosArvore);
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
	// vizinho na linha de cima
	if (zeroPos.x - 1 >= 0) {
		int matrizTemp[4][4];
		cloneArray(nodeOrigem->matriz, matrizTemp);
		matrizTemp[zeroPos.x][zeroPos.y] = matrizTemp[zeroPos.x - 1][zeroPos.y];
		matrizTemp[zeroPos.x - 1][zeroPos.y] = 0;
		qtaFilhos++;
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, nodesInseridosArvore))){
			node *filhoCima = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoCima->matriz);
			filhoCima->matriz[zeroPos.x][zeroPos.y] = filhoCima->matriz[zeroPos.x - 1][zeroPos.y];
			filhoCima->matriz[zeroPos.x - 1][zeroPos.y] = 0;
			inicializaFilho(filhoCima);
			insereListaLigada(filhoCima, &nodeOrigem->filhos);
			insereListaLigada(filhoCima, &nodesInseridosArvore);
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
	// vizinho na linha de baixo
	if (zeroPos.x + 1 < 4) {
		int matrizTemp[4][4];
		cloneArray(nodeOrigem->matriz, matrizTemp);
		matrizTemp[zeroPos.x][zeroPos.y] = matrizTemp[zeroPos.x + 1][zeroPos.y];
		matrizTemp[zeroPos.x + 1][zeroPos.y] = 0;
		qtaFilhos++;
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, nodesInseridosArvore))){
			node *filhoBaixo = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoBaixo->matriz);
			filhoBaixo->matriz[zeroPos.x][zeroPos.y] = filhoBaixo->matriz[zeroPos.x + 1][zeroPos.y];
			filhoBaixo->matriz[zeroPos.x + 1][zeroPos.y] = 0;
			inicializaFilho(filhoBaixo);
			insereListaLigada(filhoBaixo, &nodeOrigem->filhos);
			insereListaLigada(filhoBaixo, &nodesInseridosArvore);
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
	nodeOrigem->qtaFilhos = qtaFilhos;
}

void inicializaFormigas(formiga *formiga, int index, node *raiz){
	formiga->id = index;
	formiga->caminho = NULL;
	formiga->movimentos = 0;
	formiga->resolvido = 1;
	insereListaLigada(raiz, &formiga->caminho);
}

void inicializaArvore(node *raiz){
	cloneArray(matrizInicial, raiz->matriz);
	raiz->valorHeuristica = 1;
	raiz->feromonio = 1;
	raiz->filhos = NULL;
	insereListaLigada(raiz, &nodesInseridosArvore);
}

// escolhe probabilisticamente o melhor dos filhos
node* escolheFilho(node *nodeAtual){ 
	return selecaoRoleta(nodeAtual->filhos, nodeAtual->qtaFilhos, alfa, beta);
}

 // adiciona o filho escolhido gerado no caminho
void adicionaNoCaminho(formiga *formiga, node *filho){
	if (estaNoCaminho(filho->matriz, formiga) == 0){
		insereListaLigada(filho, &formiga->caminho);
		formiga->movimentos += 1;
	} else {
		//printf("Ta no caminho\n");
	}
}

void geraSolucao(formiga *formiga, node *raiz) {
	while (matrizIgual(matrizResposta, formiga->caminho->nodeAtual->matriz) != 1){
		if (formiga->caminho->nodeAtual->filhos == NULL){
			geraNode(formiga->caminho->nodeAtual);
			if (todosNoCaminho(formiga)){
				formiga->resolvido = 0;
				break;
			}
		}
		node *filho = escolheFilho(formiga->caminho->nodeAtual);
		adicionaNoCaminho(formiga, filho);

		if (formiga->movimentos >= ALTURA_ARVORE_MAX) {
			formiga->resolvido = 0;
			break;
		}
	}
	printf("Formiga resolveu? %d\n", formiga->resolvido);	
	printf("movimentos finais: %d\n", formiga->movimentos);
}

void freeFormigas(formiga formigas[]) {
	int i;
	for (i = 0; i < QTA_FORMIGAS; i++){
		listaLigada *atual = formigas[i].caminho;
		listaLigada *anterior;
		while (atual != NULL){
			anterior = atual;
			atual = atual->prev;
			free(anterior);
		}
	}
}

int antsystem(){
	formiga formigas[QTA_FORMIGAS];
	int i, contadorCiclos = 0;
	inicializaArvore(&raizArvore);
	int melhorMovimentos = INT_MAX;
	while (contadorCiclos != MAX_CICLOS){
		for (i = 0; i < QTA_FORMIGAS; i++){
			inicializaFormigas(&formigas[i], i, &raizArvore);
			geraSolucao(&formigas[i], &raizArvore);
		}
		for (i = 0; i < QTA_FORMIGAS; i++){
			if (matrizIgual(formigas[i].caminho->nodeAtual->matriz, matrizResposta)){
				if (formigas[i].movimentos < melhorMovimentos){
					melhorMovimentos = formigas[i].movimentos;
				}			
			}
			atualizaFeromonioCaminho(&formigas[i]);
		}
		printf("Final do Ciclo %d\n", contadorCiclos);
		freeFormigas(formigas);
		contadorCiclos++;
		atualizaFeromonioGlobal();
	}
	return melhorMovimentos;
}

int main(){
	inicializaMatrizResposta(matrizResposta);

	leEntrada("entradas/med/18mov.txt", matrizInicial);
	printf("\n\n");

	unsigned long long seed = time(NULL);
	inicializaRandom(seed);


	printf("Movimentos: %d\n", antsystem());
	printf("Tempo: %llu\n", (time(NULL) - seed));
}
// testes: ftp://ftp.cs.princeton.edu/pub/cs226/8puzzle