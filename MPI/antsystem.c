#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <ctype.h>
#include "leitor.h"
#include "utils.h"
#include "estruturas.h"
#include "heuristica.h"

#define NODE_ARVORE_MAX 3000000
#define MAX_BUCKETS_ARVORE 64000
#define MAX_BUCKETS_CAMINHO 128

int matrizInicial[4][4];
int matrizResposta[4][4] = {	  
	{1, 2, 3 ,4}, 
	{5, 6, 7, 8},
	{9, 10, 11, 12}, 
	{13, 14, 15, 0}
};

listaLigada *queue;
listaLigada *queueHead;
node raizArvore;
hashmap nodesInseridosArvore;
int solucaoNaArvore = 0;

int heuristicaUsada = 0;
double alfa = 0.1;
double beta = 1;
double rho = 0.5;
int maxCiclos = 5;
int qtaFormigas = 500;
int nThreads = 1;
char *arquivo = NULL;
unsigned long long seed = 0;

pthread_barrier_t barreira;
pthread_mutex_t lock;

int globalMelhorMovimentos = INT_MAX;
int contadorCiclos = 0;

void atualizaFeromonioCaminho(formiga *formiga){
	double delta;
	delta = 16.0 / formiga->movimentos;
	listaLigada *atual = formiga->caminho.todos;
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
	listaLigada *atual = nodesInseridosArvore.todos;
	double limiteInferior = 0.01;
	while (atual != NULL){
		nodesInseridosArvore.todos->nodeAtual->feromonio *= rho;
		if (nodesInseridosArvore.todos->nodeAtual->feromonio <= 0) 
			nodesInseridosArvore.todos->nodeAtual->feromonio = limiteInferior;
		atual = atual->prev;
	}
}

void inicializaFilho(node *filho){
	filho->valorHeuristica = calculaHeuristica(matrizResposta, filho->matriz, heuristicaUsada);
	filho->feromonio = 1;
	filho->filhos = NULL;
}

node* criaFilho(node *nodePai, char direcao) {
	par zeroPos;
	zeroPos = achaPosicaoZero(nodePai->matriz);	

	node *filho = malloc(sizeof(node));
	cloneArray(nodePai->matriz, filho->matriz);

	switch (direcao) {
		case 'e':
			filho->matriz[zeroPos.x][zeroPos.y] = filho->matriz[zeroPos.x][zeroPos.y - 1];
			filho->matriz[zeroPos.x][zeroPos.y - 1] = 0;
			break;
		case 'd':
			filho->matriz[zeroPos.x][zeroPos.y] = filho->matriz[zeroPos.x][zeroPos.y + 1];
			filho->matriz[zeroPos.x][zeroPos.y + 1] = 0;
			break;
		case 'c':
			filho->matriz[zeroPos.x][zeroPos.y] = filho->matriz[zeroPos.x - 1][zeroPos.y];
			filho->matriz[zeroPos.x - 1][zeroPos.y] = 0;
			break;
		case 'b':
			filho->matriz[zeroPos.x][zeroPos.y] = filho->matriz[zeroPos.x + 1][zeroPos.y];
			filho->matriz[zeroPos.x + 1][zeroPos.y] = 0;
			break;
	}
	inicializaFilho(filho);
	insereListaLigada(filho, &nodePai->filhos);
	listaLigada *newHead = insereListaLigada(filho, &queue);
	if (newHead)
		queueHead = newHead;
	insereHash(filho, &nodesInseridosArvore);
	return filho;
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
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, &nodesInseridosArvore))) {
			criaFilho(nodeOrigem, 'e');
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
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, &nodesInseridosArvore))){
			criaFilho(nodeOrigem, 'd');
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
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, &nodesInseridosArvore))){
			criaFilho(nodeOrigem, 'c');
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
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, &nodesInseridosArvore))) {
			criaFilho(nodeOrigem, 'b');
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
	if (matrizIgual(matrizResposta, nodeOrigem->matriz)) 
		solucaoNaArvore = 1;

	nodeOrigem->qtaFilhos = qtaFilhos;
}

void inicializaFormigas(formiga *formiga, int index, node *raiz){
	formiga->id = index;
	inicializaHash(&formiga->caminho, MAX_BUCKETS_CAMINHO);
	formiga->movimentos = 0;
	formiga->resolvido = 1;
	insereHash(raiz, &formiga->caminho);
}

void inicializaArvore(node *raiz){
	cloneArray(matrizInicial, raiz->matriz);
	raiz->valorHeuristica = 1;
	raiz->feromonio = 1;
	raiz->filhos = NULL;
	insereHash(raiz, &nodesInseridosArvore);
}

node* escolheFilho(node *nodeAtual){ 
	return selecaoRoleta(nodeAtual->filhos, nodeAtual->qtaFilhos, alfa, beta);
}

void adicionaNoCaminho(formiga *formiga, node *filho){
	if (!getNoCaminhoExiste(filho->matriz, &formiga->caminho)){
		insereHash(filho, &formiga->caminho);
		formiga->movimentos += 1;
	}
}

void geraSolucao(formiga *formiga, node *raiz) {  	
  	while (!matrizIgual(matrizResposta, formiga->caminho.todos->nodeAtual->matriz)){
		if (todosNoCaminho(formiga)){
			formiga->resolvido = 0;
			break;
		}
		node *filho = escolheFilho(formiga->caminho.todos->nodeAtual);
		adicionaNoCaminho(formiga, filho);
	}
	//if (formiga->resolvido)
	//	printf("Achou solucao com %d movimentos\n", formiga->movimentos);
}

void freeFormigas(formiga formigas[]) {
	int i, j;
	for (i = 0; i < qtaFormigas/nThreads; i++){
		listaLigada *atual = formigas[i].caminho.todos;
		listaLigada *anterior;
		while (atual != NULL){
			anterior = atual;
			atual = atual->prev;
			free(anterior);
		}
		for (j = 0; j < MAX_BUCKETS_CAMINHO; j++){
			listaLigada *atual = formigas[i].caminho.buckets[j];
			listaLigada *anterior;
			while (atual != NULL){
				anterior = atual;
				atual = atual->prev;
				free(anterior);
			}
		}
		free(formigas[i].caminho.buckets);
	}	
}

void *antsystem(void *threadId){
	formiga formigas[qtaFormigas/nThreads];

	int i;
	long id;
	id = *(long*)threadId;
	int melhorLocal = INT_MAX;

	while (contadorCiclos != maxCiclos){    
		for (i = 0; i < qtaFormigas/nThreads; i++){
			inicializaFormigas(&formigas[i], i, &raizArvore);
			geraSolucao(&formigas[i], &raizArvore);
		}
		for (i = 0; i < qtaFormigas/nThreads; i++){
			if (matrizIgual(formigas[i].caminho.todos->nodeAtual->matriz, matrizResposta)){
				if (formigas[i].movimentos < melhorLocal){
					melhorLocal = formigas[i].movimentos;
				}			
			}
		}
		//printf("Thread %li esperando\n", id);
		pthread_barrier_wait(&barreira);

		for (i = 0; i < qtaFormigas/nThreads; i++){
			pthread_mutex_lock(&lock);
			atualizaFeromonioCaminho(&formigas[i]);
			pthread_mutex_unlock(&lock);
		}
		if (id == 0){
			//printf("Final do Ciclo %d\n", contadorCiclos);
			contadorCiclos++;
			atualizaFeromonioGlobal();	
		}
		freeFormigas(formigas);    
		pthread_barrier_wait(&barreira); 
	}
	pthread_mutex_lock(&lock);
	if (globalMelhorMovimentos > melhorLocal)
		globalMelhorMovimentos = melhorLocal;
	pthread_mutex_unlock(&lock);
	return NULL;
}

void geraArvore() {
	inicializaArvore(&raizArvore);

	node *atual = &raizArvore;
	queueHead = insereListaLigada(atual, &queue);
	while (queueHead) {
		atual = removeListaLigada(&queueHead, &queue);
		geraNode(atual);
		if (nodesInseridosArvore.qtaNodes >= NODE_ARVORE_MAX || solucaoNaArvore)
			break;
	}
	if (solucaoNaArvore)
		printf("Solucao na arvore\n");
}

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
    printf("  -s <seed>          seed para o random\n\n");
}

int main(int argc, char **argv){
	char c;
    while ((c = getopt (argc, argv, "a:b:r:n:f:c:s:t:h")) != -1){
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
	            qtaFormigas = atoi(optarg);
	            break;
	        case 'c':
	            maxCiclos = atoi(optarg);
	            break;
	        case 's':
	            seed = atoi(optarg);
	            break;
	        case 'f':
	            arquivo = optarg;
	            break;
	        case 't':
	            nThreads = atoi(optarg);
	            if (qtaFormigas % nThreads != 0){
	            	printf("%d\n", qtaFormigas/nThreads);
	            	printf("A quantidade de threads deve ser divisivel pelo numero de formigas\n");
	            	exit(1);
	            } 
	            break;
	        case 'h':
	            imprimeUsage();
	            exit(0);
	        case '?':
	            printf("Erro\n");
	        default:
	            abort();
	    }
	}
    printf("\nAlfa = %.2f, Beta = %.2f, Rho = %.2f, Formigas = %d, Ciclos = %d, Arquivo = %s, Threads = %d\n",
        alfa, beta, rho, qtaFormigas, maxCiclos, arquivo, nThreads);

	inicializaHash(&nodesInseridosArvore, MAX_BUCKETS_ARVORE);
	leEntrada(arquivo, matrizInicial);
	printf("\n\n");
	if (seed == 0)
		seed = time(NULL);
 	unsigned long long tempoExecucaoTotal = time(NULL);
	inicializaRandom(seed);

	printf("Gerando Arvore...\n");
	geraArvore();
	printf("Arvore gerada, começando antsystem\n");

	pthread_barrier_init(&barreira, NULL, nThreads);
	pthread_mutex_init(&lock, NULL);
	pthread_t threads[nThreads]; 
	long threadId[nThreads]; 

	long i;
	int err = 0;
	
 	unsigned long long tempoExecucaoAlgoritmo = time(NULL);
 	if (solucaoNaArvore) {
	 	for (i = 0; i < nThreads; i++) {
			threadId[i] = i;
			err = pthread_create(&threads[i], NULL, antsystem, (void*)&threadId[i]);
		}
		if (err)
			printf("Erro ao criar thread %d\n", err);

		for (i = 0; i < nThreads; i++)
			pthread_join(threads[i++], NULL);
	} else {
		printf("Solucao nao esta na arvore\n");
	}	
    pthread_mutex_destroy(&lock);
    pthread_barrier_destroy(&barreira);

	printf("\n\nResumo\n");
	printf("Formigas: %d\n", qtaFormigas);
	printf("Formigas por Thread: %d\n", qtaFormigas/nThreads);
	printf("Ciclos: %d\n", maxCiclos);
	if (globalMelhorMovimentos == INT_MAX)
		printf("Solucao Encontrada: Nenhuma\n");
	else
		printf("Solucao Encontrada: %d\n", globalMelhorMovimentos);
	printf("Tempo Gerando Arvore: %llus\n", (tempoExecucaoAlgoritmo - tempoExecucaoTotal));
	printf("Tempo Ant System: %llus\n", (time(NULL) - tempoExecucaoAlgoritmo));	
	printf("Tempo Total: %llus\n", (time(NULL) - tempoExecucaoTotal));
	printf("Nodes na Arvore: %d\n", nodesInseridosArvore.qtaNodes);
}
