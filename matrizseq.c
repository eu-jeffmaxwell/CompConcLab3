#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat; //matriz que será carregada do arquivo
float *saida; //vetor de saida
//descomentar o define abaixo caso deseje imprimir uma versao truncada da matriz gerada no formato texto
//#define TEXTO 


int main(int argc, char* argv[]){

    int linhas, colunas; //dimensoes da matriz
    double inicio, fim, delta;
    long long int tam; //qtde de elementos na matriz
    FILE * arq; //descritor do arquivo de entrada
    size_t ret; //retorno da funcao de leitura no arquivo de entrada
    
    GET_TIME(inicio);
    //Leitura e avaliação dos parametros de entrada
    if(argc < 3){
        printf(" Digite: %s <Arquivo de entrada> <Arquivo de saida>\n", argv[0]);
        return 1;
    }

    //abre o arquivo para leitura binaria
    arq = fopen(argv[1], "rb");
    if(!arq) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
    }
    
    //le as dimensoes da matriz
    ret = fread(&linhas, sizeof(int), 1, arq);
    if(!ret) {fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");return 3;}
    ret = fread(&colunas, sizeof(int), 1, arq);
    if(!ret) {fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");return 3;}
    tam = linhas * colunas; //calcula a qtde de elementos da matriz
  
    //Alocação de memória para as estruturas de dados
    mat = (float *) malloc(sizeof(float) * tam);
    if(mat == NULL){printf("ERRO-- malloc1\n"); return 3;}
    saida = (float *) malloc(sizeof(float) * tam);
    if(saida == NULL) {printf("ERRO-- malloc3\n"); return 3;}

    //carrega a matriz de elementos do tipo float do arquivo
    ret = fread(mat, sizeof(float), tam, arq);
    if(ret < tam) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }

    GET_TIME(fim);
    delta = fim -inicio;
    printf("Tempo de inicialização: %f\n", delta);

    GET_TIME(inicio);
    // Função para multiplicar duas matrizes quadradas representadas como vetores
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            for (int k = 0; k < colunas; k++) {
                saida[i * colunas + j] += mat[i * colunas + k] * mat[k * colunas + j];
            }
        }
    }
    
    GET_TIME(fim);
    delta = fim -inicio;
    printf("Tempo de multiplicação: %f\n", delta);

    fclose(arq);
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
        return 3;
    }

    //escreve numero de linhas e de colunas
    ret = fwrite(&linhas, sizeof(int), 1, arq);
    ret = fwrite(&colunas, sizeof(int), 1, arq);
    //escreve os elementos da matriz
    ret = fwrite(saida, sizeof(float), tam, arq);
    if(ret < tam) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }
    
    //Liberação da memória
    //finaliza o uso das variaveis
     //liberacao da memoria
    GET_TIME(inicio);
    fclose(arq);
    free(saida);
    free(mat);
    GET_TIME(fim)   
    delta = fim - inicio;
    printf("Tempo finalizacao:%lf\n", delta);
    return 0;
}
