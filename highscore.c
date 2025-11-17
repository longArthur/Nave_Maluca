#include <stdio.h>

#define MAXSCORES 10

typedef struct{
    char nome[45];
    int pontos;
}SCORE;

int read_highscore(char path[], int scores[MAXSCORES]){
    FILE *arq;
    int result_leitura, i; //variavel para verificar se leitura deu certo
    
    if(!(arq = fopen(path,"rb"))) // abre para leitura e testa leitura
        return 0;
    else{
        result_leitura = fread(scores, sizeof(SCORE), MAXSCORES, arq);
        fclose(arq);
    }
    if (result_leitura <= MAXSCORES) // teste da leitura
        return 0;
    
        
    return 1;
}

int write_highscore(char path[], SCORE scores[MAXSCORES], int num_scores){
    FILE *arq;
    int result_escrita;

    if(!(arq = fopen(path, "wb"))){ //abre para escrita binaria e testa a abertura
        printf("Erro na abertura do arquivo.");
        return 0;
    }
    else{
        result_escrita = fwrite(scores, sizeof(SCORE), num_scores, arq);
        fclose(arq);
        if(result_escrita != num_scores){ //testa se escreveu certo
            printf("Erro na escrita do arquivo.");
            return 0;
        }
    }

    return 1;
}

/*
int le_arquivo(char nome_arq[], JOGADOR top5[MAXSCORES]){
    FILE *arq;
    int result_leitura, i; //variavel para verificar se leitura deu certo

    if(!(arq = fopen(nome_arq,"rb"))) // abre para leitura e testa leitura
        return 0;
    else{
        result_leitura = fread(top5, sizeof(JOGADOR), MAXSCORES, arq);
        fclose(arq);
    }
    if (result_leitura != MAXSCORES) // teste da leitura
        return 0;
    
        
    
    return 1;
}

*/