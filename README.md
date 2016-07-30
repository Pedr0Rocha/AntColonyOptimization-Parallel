# Any Colony Optimization (ACO) - Programação Concorrente

##### Pedro Rocha - RA 68740

### 1. Instruções
O projeto possui três implementações do algoritmo, uma sequencial, paralelo usando pthreads e usando MPI (memória compartilhada). 
##### 1.1 Compilar
Para compilar basta entrar na pasta do respectivo algoritmo e digitar os comandos 
```
$ make clear
$ make
```
##### 1.2 Executar
Para executar o programa após compilado basta digitar o comando abaixo. O algoritmo irá executar com os parâmetros usados para testes.
```
$ ./make run
```
### 2. Parâmetros
O algoritmo possui 6 parâmetros
- -a <alfa>
   * Peso dado ao feromônio
- -b <beta>
   * Peso dado a heurística
- -r <rho>
   * Taxa de evaporação do feromônio
- -n <n_formigas>
   * Número de formigas
- -f <arquivo>
   * Arquivo de Entrada
- -s <seed>
   * Seed para o random
- -t <n_threads>
   * Número de threads

Para ver as dicas de uso no programa digite
```
$ ./<programa> -h
```
Parâmetros usado para testes: -a 0.1 -b 1 -r 0.5 -n 1000 -f entradas/14mov.txt -c 1000
