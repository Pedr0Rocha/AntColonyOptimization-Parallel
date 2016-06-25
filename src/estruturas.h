#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct node node;
typedef struct listaLigada listaLigada;
typedef struct hashmap hashmap;

struct node {
	int matriz[4][4];
	int valorHeuristica;
	float feromonio;
	int qtaFilhos;
	listaLigada *filhos;
};

struct listaLigada {
	node *nodeAtual;
	listaLigada *prev;
};
struct hashmap {
	listaLigada **buckets;
	listaLigada *todos;
	int qtaNodes;
	int qtaBuckets;
};

typedef struct {
	int id;
	hashmap caminho;
	int movimentos;
	int resolvido;
} formiga;

typedef struct {
	int x, y;
} par;

int calculaQuantidadeFilhos(node *node);
int matrizIgual(int matrizAlvo[4][4], int matrizComparar[4][4]);
void imprimeMatriz(int matriz[4][4]);
void cloneArray(int matriz[4][4], int clone[4][4]);
void insereListaLigada(node *node, listaLigada **lista);
node* getFilho(int i, listaLigada *filhos);
node* getNoCaminhoExiste(int matriz[4][4], hashmap *hash);
int todosNoCaminho(formiga *formiga);
par achaPosicaoZero(int matriz[4][4]);
void imprimeFilhosNode(node *node);
void imprimeNode(node *node);
int geraHashKey(int matriz[4][4], int qtaBuckets);
node* buscaHash(int matriz[4][4], hashmap *map);
void insereHash(node *node, hashmap *map);
void inicializaHash(hashmap *map, int qtaBuckets);

#endif