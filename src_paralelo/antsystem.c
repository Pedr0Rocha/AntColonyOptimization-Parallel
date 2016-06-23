#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include "leitor.h"
#include "utils.h"
#include "estruturas.h"
#include "heuristica.h"

#define MAX_CICLOS 5
#define QTA_FORMIGAS 40
#define ALTURA_ARVORE_MAX 100
#define N_THREADS 2

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

pthread_barrier_t barreira;
pthread_mutex_t lock;


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
			// regiao critica em todos os insereListaLigada
    		pthread_mutex_lock(&lock);
			insereListaLigada(filhoEsquerda, &nodeOrigem->filhos);
			insereListaLigada(filhoEsquerda, &nodesInseridosArvore);
    		pthread_mutex_unlock(&lock);
		} else {
    		pthread_mutex_lock(&lock);
			insereListaLigada(ptNode, &nodeOrigem->filhos);
    		pthread_mutex_unlock(&lock);
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
    		pthread_mutex_lock(&lock);
			insereListaLigada(filhoDireita, &nodeOrigem->filhos);
			insereListaLigada(filhoDireita, &nodesInseridosArvore);
    		pthread_mutex_unlock(&lock);
		} else {
    		pthread_mutex_lock(&lock);
			insereListaLigada(ptNode, &nodeOrigem->filhos);
    		pthread_mutex_unlock(&lock);
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
    		pthread_mutex_lock(&lock);
			insereListaLigada(filhoCima, &nodeOrigem->filhos);
			insereListaLigada(filhoCima, &nodesInseridosArvore);
    		pthread_mutex_unlock(&lock);
		} else {
    		pthread_mutex_lock(&lock);
			insereListaLigada(ptNode, &nodeOrigem->filhos);
    		pthread_mutex_unlock(&lock);
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
    		pthread_mutex_lock(&lock);
			insereListaLigada(filhoBaixo, &nodeOrigem->filhos);
			insereListaLigada(filhoBaixo, &nodesInseridosArvore);
    		pthread_mutex_unlock(&lock);
		} else {
    		pthread_mutex_lock(&lock);
			insereListaLigada(ptNode, &nodeOrigem->filhos);
    		pthread_mutex_unlock(&lock);
		}
	}
    pthread_mutex_lock(&lock);
	nodeOrigem->qtaFilhos = qtaFilhos;
   	pthread_mutex_unlock(&lock);
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
			// regiao critica, adiciona filhos na arvore global
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
			printf("Starvation.\n");
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

/* TODO 
- refatorar a funcao antsystem para ser chamada por threads 
- dividir formigas de acordo com threads
- tratar regioes criticas
- procurar condicoes de corrida
- testar barreira
- implementar tree barrier
- otimizar - overhead
*/
int melhorMovi = 0;
void *antsystem(void *threadId){
	formiga formigas[QTA_FORMIGAS/N_THREADS];
	int i, contadorCiclos = 0;
	long id;
	id = (long)threadId;
	if (id == 0)
		inicializaArvore(&raizArvore);

	int melhorMovimentos = INT_MAX;
	while (contadorCiclos != MAX_CICLOS){
		for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
			inicializaFormigas(&formigas[i], i, &raizArvore);
			geraSolucao(&formigas[i], &raizArvore);
		}
		for (i = 0; i < QTA_FORMIGAS/N_THREADS; i++){
			if (matrizIgual(formigas[i].caminho->nodeAtual->matriz, matrizResposta)){
				if (formigas[i].movimentos < melhorMovimentos){
					// regiao critica
					pthread_mutex_lock(&lock);
					melhorMovimentos = formigas[i].movimentos;
					pthread_mutex_unlock(&lock);
				}			
			}
			// regiao critica pq o feromonio a ser atualizado eh global
			pthread_mutex_lock(&lock);
			atualizaFeromonioCaminho(&formigas[i]);
			pthread_mutex_unlock(&lock);
		}
		// todas as threads finalizadas a partir daqui
		// atualizacao global do feromonio e contagem de ciclos
		pthread_barrier_wait(&barreira);
		if (id == 0){
			printf("Final do Ciclo %d\n", contadorCiclos);
			contadorCiclos++;
			atualizaFeromonioGlobal();
		}
		//freeFormigas(formigas);
	}
	printf("Melhor Mov: %d\n", melhorMovimentos);
	return NULL;
	//return (void*)melhorMovimentos;
}

int main(int argc, char **argv){
	inicializaMatrizResposta(matrizResposta);
	pthread_barrier_init(&barreira, NULL, N_THREADS);

	leEntrada("entradas/easy/10mov.txt", matrizInicial);
	printf("\n\n");

	unsigned long long seed = time(NULL);
	inicializaRandom(seed);
	
	pthread_t threads[N_THREADS]; 

	if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Erro ao inicial Lock\n");
        exit(1);
    }

	long i;
	int err;
	for (i = 0; i < N_THREADS; i++) {
		err = pthread_create(&threads[i], NULL, antsystem, (void*)i);
	}
	if (err){
		printf("Erro ao criar thread %d\n", err);
	}
	for (i = 0; i < N_THREADS; i++)
		pthread_join(threads[i++], NULL);

    pthread_mutex_destroy(&lock);

	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Ciclos: %d\n", MAX_CICLOS);
	int solucaoEncontrada = 9999;

	printf("\n\nResumo\n");
	printf("Formigas: %d\n", QTA_FORMIGAS);
	printf("Formigas por Thread: %d\n", QTA_FORMIGAS/N_THREADS);
	printf("Ciclos: %d\n", MAX_CICLOS);
	printf("Solucao Otima: 10\n");
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