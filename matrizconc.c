#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat; //matriz de entrada
float *saida; //matriz de saida
int nthreads; //numero de threads

//descomentar o define abaixo caso deseje imprimir uma versao truncada da matriz gerada no formato texto
//#define TEXTO 

typedef struct{
    int id; //Identificador do elemento que a thread ira processar
    int dim;
    int linhas;
    int colunas; //Dimensão das estruturas de entradas
}t_Args;

//Função que as threads vão executar
void * tarefa(void *arg){
    t_Args *args =(t_Args*) arg;
    printf("thread %d \n", args-> id);
   
    for (int i = args->id; i < args->colunas; i+= nthreads) {
        for (int j = 0; j < args->colunas; j++) {
            //saida[i * args->colunas + j] = 0;  // Inicializa a posição da matriz resultado
            for (int k = 0; k < args->colunas; k++) {
                saida[i * args->colunas + j] += mat[i * args->colunas + k] * mat[k * args->colunas + j];
            }
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){

    int dim;
    int linhas, colunas; //dimensoes da matriz
    pthread_t *tid;//identificadores das Threads no sistema
    t_Args *args; //identificadores locais das threads e dimensão
    double inicio, fim, delta; //inicialização dos parametros da tomada de tempo
    FILE * arq; //descritor do arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada

    GET_TIME(inicio);
    //Leitura e avaliação dos parametros de entrada
    if(argc < 4){
        printf(" Digite: %s <Arquivo de entrada> <Arquivo de saida> <número de threads>\n", argv[0]);
        return 1;
    }

    //abre o arquivo para leitura binaria
    arq = fopen(argv[1], "rb");
    if(!arq) {fprintf(stderr, "Erro de abertura do arquivo\n");return 2;}

    //le as dimensoes da matriz
    ret = fread(&linhas, sizeof(int), 1, arq);
    if(!ret) {fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");return 3;}
    ret = fread(&colunas, sizeof(int), 1, arq);
    if(!ret) {fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");return 3;}
    dim = linhas * colunas; //calcula a qtde de elementos da matriz

    nthreads = atoi(argv[3]);
    if (nthreads > dim) nthreads=dim; //Validador de coerencia de Threads

    //Alocação de memória para as estruturas de dados
    mat = (float *) malloc(sizeof(float) * dim);
    if(mat == NULL){printf("ERRO-- malloc"); return 3;}
    saida = (float *) malloc(sizeof(int) * dim);
    if(saida == NULL) {printf("ERRO-- malloc"); return 3;}

    //carrega a matriz de elementos do tipo float do arquivo
    ret = fread(mat, sizeof(float), dim, arq);
    if(ret < dim) {fprintf(stderr, "Erro de leitura dos elementos da matriz\n");return 3;}
   
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de inicialização: %f\n", delta);
    fclose(arq);

    //Alocação das estruturas 
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL){ puts("ERRO--malloc"); return 3;}
    args = (t_Args*) malloc(sizeof(t_Args) * nthreads);
    if(tid == NULL){ puts("ERRO--malloc"); return 3;}
    
    GET_TIME(inicio);
    //Criação das threads
    for(int i = 0; i<nthreads; i++){
        (args+i)-> id = i;
        (args+i)-> dim = dim;
        (args+i)-> colunas = colunas;
        (args+i)-> linhas = linhas;
        if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
            puts("ERRO -- pthread_create"); return 4;
        }
    }

    //espera pelo termino da threads
    for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo multiplicacao (dimensao %d) (nthreads %d): %lf\n", dim, nthreads, delta);


    //imprimir na saida padrao a matriz gerada
    #ifdef TEXTO
    for(int i=0; i<linhas; i++) { 
        for(int j=0; j<colunas; j++)
            fprintf(stdout, "%.6f ", saida[i*colunas+j]);
        fprintf(stdout, "\n");
    }
    #endif

    //escreve a matriz no arquivo
    //abre o arquivo para escrita binaria
    arq = fopen(argv[2], "wb");
    if(!arq) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 4;
    }

    //escreve numero de linhas e de colunas
    ret = fwrite(&linhas, sizeof(int), 1, arq);
    ret = fwrite(&colunas, sizeof(int), 1, arq);
    //escreve os elementos da matriz
    ret = fwrite(saida, sizeof(float), dim, arq);
    if(ret < dim) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    GET_TIME(inicio);
    //Liberação da memória
    free(mat);
    free(saida);
    free(args);
    free(tid);
    GET_TIME(fim)   
    delta = fim - inicio;
    printf("Tempo finalizacao:%lf\n", delta);

    return 0;
}
