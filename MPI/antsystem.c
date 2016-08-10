#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include "includes/leitor.h"
#include "includes/utils.h"
#include "includes/estruturas.h"
#include "includes/heuristica.h"

#define NODE_ARVORE_MAX 3000000
#define MAX_BUCKETS_ARVORE 64000
#define MAX_BUCKETS_CAMINHO 128
#define MASTER 0

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
int maxCiclos = 1000;
int qtaFormigas = 1000;
int nProcessos = 2;
char *arquivo = "entradas/14mov.txt";
unsigned long long seed = 0;

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

void atualizaFeromonioMelhorFormiga(int matriz[4][4], int movimentos) {
	node *nodeAtualizar = buscaHash(matriz, &nodesInseridosArvore);
	double delta;
	delta = 16.0 / movimentos;
	double limiteInferior = 0.01;
	nodeAtualizar->feromonio = nodeAtualizar->feromonio * (1 - rho) + delta;
	if (nodeAtualizar->feromonio < limiteInferior) 
		nodeAtualizar->feromonio = limiteInferior;
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

void freeFormigas(formiga formigas[], int quantidadeFormigas) {
	for (int i = 0; i < quantidadeFormigas; i++){
		listaLigada *atual = formigas[i].caminho.todos;
		listaLigada *anterior;
		while (atual != NULL){
			anterior = atual;
			atual = atual->prev;
			free(anterior);
		}
		for (int j = 0; j < MAX_BUCKETS_CAMINHO; j++){
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

void *antsystem(int rank, int totalRanks){
	int formigaPorProcesso = qtaFormigas/nProcessos;
	formiga formigas[formigaPorProcesso];


	while (contadorCiclos != maxCiclos) {    
		formiga melhorFormiga;

		int rankDoMelhor = 0;
		int enviarVetorMatrizes = 0;
		melhorFormiga.resolvido = 0;
		int melhorLocal = INT_MAX;
		for (int i = 0; i < formigaPorProcesso; i++) {
			inicializaFormigas(&formigas[i], i, &raizArvore);
			geraSolucao(&formigas[i], &raizArvore);
		}
		for (int i = 0; i < formigaPorProcesso; i++)
			if (matrizIgual(formigas[i].caminho.todos->nodeAtual->matriz, matrizResposta))
				if (formigas[i].movimentos < melhorLocal) {
					melhorLocal = formigas[i].movimentos;
					melhorFormiga = formigas[i];
				}			
		MPI_Barrier(MPI_COMM_WORLD);

		if (rank != MASTER) {
    		MPI_Send(&melhorLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    		MPI_Recv(&melhorLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    		MPI_Recv(&rankDoMelhor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		} else {
    		/* Recebendo informacoes de todos os processos e comparando */
			int tempMelhor;
    		for (int i = 1; i < nProcessos; i++) {
    			MPI_Recv(&tempMelhor, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    			if (tempMelhor < melhorLocal) {
    				melhorLocal = tempMelhor;
    				rankDoMelhor = i;
    			}
    		}
    		
    		/* Enviando para todos os processos melhorLocal */
    		for (int i = 1; i < nProcessos; i++) {
    			MPI_Send(&melhorLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    			MPI_Send(&rankDoMelhor, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    		}
    		/* Enviando para todos os processos a flag de enviar vetor */
    		for (int i = 1; i < nProcessos; i++) {
    			if (i != rankDoMelhor) {
    				MPI_Send(&enviarVetorMatrizes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    			} else {
    				enviarVetorMatrizes = 1;
    				MPI_Send(&enviarVetorMatrizes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    				enviarVetorMatrizes = 0;
    			}
    		}
    		if (rankDoMelhor == MASTER) enviarVetorMatrizes = 1;
    	}
    	/* Recebe flag de enviar vetor */
    	if (rank != MASTER) 
    		MPI_Recv(&enviarVetorMatrizes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    	if (melhorLocal != INT_MAX) {
	    	int matrizesAtualizar[melhorLocal][16];

	    	/* Cria vetor de matrizes com a melhor formiga e enviar para todos */
	    	if (enviarVetorMatrizes) {
				listaLigada *caminho = melhorFormiga.caminho.todos;
	    		int indexVetor = 0;
	    		while (caminho->prev != NULL) {
	    			int indexAtualizar = 0;
	    			for (int j = 0; j < 4; j++)
					 	for (int k = 0; k < 4; k++)
					 		matrizesAtualizar[indexVetor][indexAtualizar++] = caminho->nodeAtual->matriz[j][k];
					indexVetor++;
	     			caminho = caminho->prev;
	    		}
	    		for (int i = 0; i < nProcessos; i++)
	    			if (i != rankDoMelhor)
	    				MPI_Send(&matrizesAtualizar, 16 * melhorLocal, MPI_INT, i, 0, MPI_COMM_WORLD);
	    	} else {
	    		MPI_Recv(&matrizesAtualizar, 16 * melhorLocal, MPI_INT, rankDoMelhor, 0, 
	    				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    	}

	    	/* Atualiza feromonio com melhor formiga e uma parte da sua propria colonia */
	    	for (int i = 0; i < formigaPorProcesso; i++) { 
				atualizaFeromonioCaminho(&formigas[i]);
			}
			
			if (rank != rankDoMelhor)
				for (int i = 0; i < melhorLocal; i++) {
					int matriz[4][4];
					MPIArrayToArray(matrizesAtualizar[i], matriz);
					atualizaFeromonioMelhorFormiga(matriz, melhorLocal);
				}
		} else {
			for (int i = 0; i < formigaPorProcesso; i++)
				atualizaFeromonioCaminho(&formigas[i]);
		}
		contadorCiclos++;
		atualizaFeromonioGlobal();	

		freeFormigas(formigas, formigaPorProcesso);  
		if (rank == MASTER) 
			if (globalMelhorMovimentos > melhorLocal)
				globalMelhorMovimentos = melhorLocal;
		MPI_Barrier(MPI_COMM_WORLD);
	}
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
    printf("  -s <seed>          seed para o random\n\n");
}

int main(int argc, char **argv){
	char c;
    while ((c = getopt (argc, argv, "a:b:r:n:f:c:s:h")) != -1){
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
	        case 'h':
	            imprimeUsage();
	            exit(0);
	        case '?':
	            printf("Erro\n");
	        default:
	            abort();
	    }
	}
    printf("\nAlfa = %.2f, Beta = %.2f, Rho = %.2f, Formigas = %d, Ciclos = %d, Arquivo = %s\n",
        alfa, beta, rho, qtaFormigas, maxCiclos, arquivo);

	inicializaHash(&nodesInseridosArvore, MAX_BUCKETS_ARVORE);
	leEntrada(arquivo, matrizInicial);
	printf("\n\n");
	
 	unsigned long long tempoExecucaoTotal = time(NULL);
	
	int totalRanks;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);

	if (seed == 0)
		seed = time(NULL);
 	inicializaRandom(seed+rank);

	nProcessos = totalRanks;
	if (qtaFormigas % nProcessos != 0){
		printf("Numero de processos deve ser divisivel pela quantidade de formigas\n");
		exit(1);
	}

	printf("Gerando Arvore...\n");
	geraArvore();
	printf("Arvore gerada, começando antsystem\n");

 	unsigned long long tempoExecucaoAlgoritmo = time(NULL);
 
 	antsystem(rank, totalRanks);

 	if (rank == MASTER) {
		printf("\n\nResumo\n");
		printf("Formigas: %d\n", qtaFormigas);
		printf("Formigas por Processo: %d\n", qtaFormigas/nProcessos);
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
	printf("PROC %d - TERMINOU\n", rank);
	MPI_Finalize();
}

	/* 
		Passo 1:
		Se não for o MASTER 
			envia o melhor local para master
		Se for o MASTER
			recebe todos os melhores locais e pega o rank do melhor
		
		Passo 2:
		Se não for o MASTER
			recebe 0 ou 1 para flag de enviar vetor com matrizes
		Se for o MASTER
			envia 1 para quem deve enviar vetor, 0 para quem deve apenas escutar

		Passo 3:
		Quem recebeu tem a flag enviar setada, preenche o vetor com as matrizes e envia
		para todos os outros processos

		Quem nao tem a flag setada, recebe vetor com matriz para atualizacao

	*/