#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "leitor.h"
#include "utils.h"
#include "estruturas.h"
#include "heuristica.h"

#define MAX_CICLOS 10
#define QTA_FORMIGAS 100
#define ALTURA_ARVORE_MAX 100

int matrizInicial[4][4];
int matrizResposta[4][4];

node raizArvore;
listaLigada *nodesInseridosArvore = NULL;

// heuristica usada, 1 - manhattan distance, 0 - order
int heuristicaUsada = 0;

// peso aplicado no feromonio
double alfa = 0.1;
// peso aplicado na heuristica
double beta = 1;
// taxa de evapocarao
double rho = 0.5;



void atualizaFeromonioCaminho(formiga *formiga){
	double delta;
	delta = 16.0 / formiga->movimentos;
	listaLigada *atual = formiga->caminho;
	double limiteInferior = 0.01;
	while (atual != NULL){
		if (formiga->resolvido)
			atual->nodeAtual->feromonio = atual->nodeAtual->feromonio * (1 - rho) + delta;
		else 
			atual->nodeAtual->feromonio *= 0.3;			

		if (atual->nodeAtual->feromonio < limiteInferior) 
			atual->nodeAtual->feromonio = limiteInferior;
		atual = atual->prev;
	}
}

void atualizaFeromonioGlobal(){
	listaLigada *atual = nodesInseridosArvore;
	double limiteInferior = 0.01;
	while (atual != NULL){
		nodesInseridosArvore->nodeAtual->feromonio *= rho;
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

// adiciona o filho escolhido no caminho
void adicionaNoCaminho(formiga *formiga, node *filho){
	if (estaNoCaminho(filho->matriz, formiga) == 0){
		insereListaLigada(filho, &formiga->caminho);
		formiga->movimentos += 1;
	} else {
		//printf("Ta no caminho\n");
	}
}

void geraSolucao(formiga *formiga, node *raiz) {
	int estagnou = 0;
	int movAnterior = -1;
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
		if (movAnterior == formiga->movimentos)
			estagnou++;
		else 
			estagnou = 0;

		if (estagnou > 100000){
			formiga->resolvido = 0;
			printf("Formiga estagnou.\n");
			break;
		}
		movAnterior = formiga->movimentos;
	}
	if (formiga->resolvido)
		printf("Achou solucao com %d movimentos\n", formiga->movimentos);
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

int main(int argc, char **argv){
	inicializaMatrizResposta(matrizResposta);

	leEntrada("entradas/hard/25mov.txt", matrizInicial);
	printf("\n\n");

	unsigned long long seed = time(NULL);
	inicializaRandom(seed);

	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Ciclos: %d\n", MAX_CICLOS);
	int solucaoEncontrada = antsystem();

	printf("\n\nResumo\n");
	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Ciclos: %d\n", MAX_CICLOS);
	printf("Solucao Otima: 25\n");
	printf("Solucao Encontrada: %d\n", solucaoEncontrada);
	printf("Tempo: %llu\n", (time(NULL) - seed));
}
// testes: ftp://ftp.cs.princeton.edu/pub/cs226/8puzzle

/*
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
 
float alfa;
float beta;
float rho;
int formigas;
int ciclos;
int threads;
char *arquivo = NULL;
 
char c;

void imprimeUsage(){
    printf("Help\n\n");
    printf("Uso: ./a.out <parametro> <valor>\n\n");
    printf("  -a <alfa>          peso dado ao feromonio\n");
    printf("  -b <beta>          peso dado a heuristica\n");
    printf("  -r <rho>           taxa de evaporacao do feromonio\n");
    printf("  -n <n_formigas>    numero de formigas\n");
    printf("  -f <arquivo>       caminho para o arquivo de entrada\n");
    printf("  -c <ciclos>        numero de ciclos\n");
    printf("  -t <threads>       numero de threads\n\n");
}

Getopt
    while ((c = getopt (argc, argv, "a:b:r:n:f:c:t:h")) != -1 || sai){
        switch (c){
            case 'a':
                alfa = atof(optarg);
                break;
            case 'b':
                beta = atof(optarg);
                break;
            case 'r':
                rho = atof(optarg);
                break;
            case 'n':
                formigas = atof(optarg);
                break;
            case 'c':
                ciclos = atof(optarg);
                break;
            case 'f':
                arquivo = optarg;
                break;
            case 't':
                threads = atof(optarg);
                break;
            case 'h':
                imprimeUsage();
                sai = 1;
                break;
            case '?':
                printf("Erro\n");
            default:
                abort();
        }
    }

    printf("alfa = %f, beta = %f, rho = %f, formigas = %d, ciclos = %d, arquivo = %s, threads = %d\n",
        alfa, beta, rho, formigas, ciclos, arquivo, threads);
*/