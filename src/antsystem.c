#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "leitor.h"
#include "utils.h"
#include "estruturas.h"
#include "heuristica.h"

#define MAX_CICLOS 5
#define QTA_FORMIGAS 500
#define ALTURA_ARVORE_MAX 300
#define N_THREADS 1
#define MAX_BUCKETS_ARVORE 80000
#define MAX_BUCKETS_CAMINHO 128

int matrizInicial[4][4];
int matrizResposta[4][4];

node raizArvore;
hashmap nodesInseridosArvore;

// heuristica usada, 1 - manhattan distance, 0 - order
int heuristicaUsada = 0;

// peso aplicado no feromonio
double alfa = 0.1;
// peso aplicado na heuristica
double beta = 1;
// taxa de evapocarao
double rho = 0.5;

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
		else {
			atual->nodeAtual->feromonio *= 0.3;			
		}

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
			node *filhoEsquerda = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoEsquerda->matriz);
			filhoEsquerda->matriz[zeroPos.x][zeroPos.y] = filhoEsquerda->matriz[zeroPos.x][zeroPos.y - 1];
			filhoEsquerda->matriz[zeroPos.x][zeroPos.y - 1] = 0;
			inicializaFilho(filhoEsquerda);
			insereListaLigada(filhoEsquerda, &nodeOrigem->filhos);
			insereHash(filhoEsquerda, &nodesInseridosArvore);
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
			node *filhoDireita = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoDireita->matriz);
			filhoDireita->matriz[zeroPos.x][zeroPos.y] = filhoDireita->matriz[zeroPos.x][zeroPos.y + 1];
			filhoDireita->matriz[zeroPos.x][zeroPos.y + 1] = 0;
			inicializaFilho(filhoDireita);
			insereListaLigada(filhoDireita, &nodeOrigem->filhos);
			insereHash(filhoDireita, &nodesInseridosArvore);
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
			node *filhoCima = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoCima->matriz);
			filhoCima->matriz[zeroPos.x][zeroPos.y] = filhoCima->matriz[zeroPos.x - 1][zeroPos.y];
			filhoCima->matriz[zeroPos.x - 1][zeroPos.y] = 0;
			inicializaFilho(filhoCima);
			insereListaLigada(filhoCima, &nodeOrigem->filhos);
			insereHash(filhoCima, &nodesInseridosArvore);
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
		if (!(ptNode = getNoCaminhoExiste(matrizTemp, &nodesInseridosArvore))){
			node *filhoBaixo = malloc(sizeof(node));
			cloneArray(nodeOrigem->matriz, filhoBaixo->matriz);
			filhoBaixo->matriz[zeroPos.x][zeroPos.y] = filhoBaixo->matriz[zeroPos.x + 1][zeroPos.y];
			filhoBaixo->matriz[zeroPos.x + 1][zeroPos.y] = 0;
			inicializaFilho(filhoBaixo);
			insereListaLigada(filhoBaixo, &nodeOrigem->filhos);
			insereHash(filhoBaixo, &nodesInseridosArvore);
		} else {
			insereListaLigada(ptNode, &nodeOrigem->filhos);
		}
	}
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

// escolhe probabilisticamente o melhor dos filhos
node* escolheFilho(node *nodeAtual){ 
	return selecaoRoleta(nodeAtual->filhos, nodeAtual->qtaFilhos, alfa, beta);
}

// adiciona o filho escolhido no caminho
void adicionaNoCaminho(formiga *formiga, node *filho){
	if (!getNoCaminhoExiste(filho->matriz, &formiga->caminho)){
		insereHash(filho, &formiga->caminho);
		formiga->movimentos += 1;
	}
}

void geraSolucao(formiga *formiga, node *raiz) {
	int estagnou = 0;
	int movAnterior = -1;
	
	clock_t t, t2;
	t = clock();
  	
  	while (matrizIgual(matrizResposta, formiga->caminho.todos->nodeAtual->matriz) != 1){
		t2 = clock();
		t = clock();
		pthread_mutex_lock(&lock);
		if (formiga->caminho.todos->nodeAtual->filhos == NULL)
			geraNode(formiga->caminho.todos->nodeAtual);	
		pthread_mutex_unlock(&lock);
		t = clock() - t;
		printf("Tempo no Gera Node: %f segundos\n", ((float)t)/CLOCKS_PER_SEC);

		t = clock();
		if (todosNoCaminho(formiga)){
			formiga->resolvido = 0;
			break;
		}
		t = clock() - t;
		printf("Tempo no Todos No Caminho: %f segundos\n", ((float)t)/CLOCKS_PER_SEC);
	
		t = clock();
		node *filho = escolheFilho(formiga->caminho.todos->nodeAtual);
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
			printf("Starvation.\n");
			break;
		}
		movAnterior = formiga->movimentos;
		t = clock() - t;
		printf("Tempo no Resto: %f segundos\n", ((float)t)/CLOCKS_PER_SEC);
		t2 = clock() - t2;
		printf("Tempo Total: %f segundos\n", ((float)t2)/CLOCKS_PER_SEC);
	}
	if (formiga->resolvido)
		printf("Achou solucao com %d movimentos\n", formiga->movimentos);
}

void freeFormigas(formiga formigas[]) {
	int i, j;
	for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
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
	formiga formigas[QTA_FORMIGAS/N_THREADS];

	int i;
	long id;
	id = *(long*)threadId;
	if (id == 0)
		inicializaArvore(&raizArvore);
	int melhorLocal = INT_MAX;

	struct timeval inicio, final, iniciowhile, finalwhile;
	int tmili;
	while (contadorCiclos != MAX_CICLOS){    
    	gettimeofday(&iniciowhile, NULL);
		for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
			inicializaFormigas(&formigas[i], i, &raizArvore);    
    		gettimeofday(&inicio, NULL);
			geraSolucao(&formigas[i], &raizArvore);
    		gettimeofday(&final, NULL);
    		tmili = (int) (1000 * (final.tv_sec - inicio.tv_sec) + (final.tv_usec - inicio.tv_usec) / 1000);
    		if (i == 0)
    			printf("Tempo da formiga %d da thread %li em gera solucao: %d\n", i, id, tmili);
		}
		for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
			if (matrizIgual(formigas[i].caminho.todos->nodeAtual->matriz, matrizResposta)){
				if (formigas[i].movimentos < melhorLocal){
					melhorLocal = formigas[i].movimentos;
				}			
			}
		}

		printf("Thread %li esperando\n", id);
		pthread_barrier_wait(&barreira);

		for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
			pthread_mutex_lock(&lock);
			atualizaFeromonioCaminho(&formigas[i]);
			pthread_mutex_unlock(&lock);
		}
		if (id == 0){
			printf("Final do Ciclo %d\n", contadorCiclos);
			contadorCiclos++;
			atualizaFeromonioGlobal();	
		}
		freeFormigas(formigas);    
    	gettimeofday(&finalwhile, NULL);
    	tmili = (int) (1000 * (finalwhile.tv_sec - iniciowhile.tv_sec) + (finalwhile.tv_usec - iniciowhile.tv_usec) / 1000);
    	printf("Tempo do While total: %d\n", tmili);
		pthread_barrier_wait(&barreira); 
	}
	pthread_mutex_lock(&lock);
	if (globalMelhorMovimentos > melhorLocal)
		globalMelhorMovimentos = melhorLocal;
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main(int argc, char **argv){
	inicializaMatrizResposta(matrizResposta);
	inicializaHash(&nodesInseridosArvore, MAX_BUCKETS_ARVORE);
	pthread_barrier_init(&barreira, NULL, N_THREADS);

	leEntrada("entradas/hard/38mov.txt", matrizInicial);
	printf("\n\n");

	unsigned long long seed = time(NULL);
	inicializaRandom(seed);
	
	pthread_t threads[N_THREADS]; 
	long threadId[N_THREADS]; 

	if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Erro ao inicial Lock\n");
        exit(1);
    }

	long i;
	int err;
	for (i = 0; i < N_THREADS; i++) {
		threadId[i] = i;
		err = pthread_create(&threads[i], NULL, antsystem, (void*)&threadId[i]);
	}
	if (err)
		printf("Erro ao criar thread %d\n", err);

	for (i = 0; i < N_THREADS; i++)
		pthread_join(threads[i++], NULL);

    pthread_mutex_destroy(&lock);
    pthread_barrier_destroy(&barreira);

	
	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Ciclos: %d\n", MAX_CICLOS);

	printf("\n\nResumo\n");
	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Formigas por Thread: %d\n", QTA_FORMIGAS/N_THREADS);
	printf("Ciclos: %d\n", MAX_CICLOS);
	printf("Solucao Otima: 38\n");
	printf("Solucao Encontrada: %d\n", globalMelhorMovimentos);
	printf("Tempo: %llu\n", (time(NULL) - seed));	
	printf("Nodes na Arvore: %d\n", nodesInseridosArvore.qtaNodes);
	// int i;
	// for (i = 0; i < 1024; i++){
	// 	int quantidade = 0;
	// 	listaLigada *atual = nodesInseridosArvore.buckets[i];
	// 	while (atual != NULL){
	// 		quantidade++;
	// 		atual = atual->prev;
	// 	}
	// 	printf("Quantidade no bucket %d: %d\n", i, quantidade);
	// }
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
