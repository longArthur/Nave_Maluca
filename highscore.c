#include <stdio.h>
#include <string.h>
#include "raylib.h"

#define MAXSCORES 10
#define SCOREPATH "highscores.bin"

typedef struct
{
    char nome[45];
    int pontos;
} SCORE;

//le a highscore do arquivo SCOREPATH e retorna em scores
//retorna quantos foram lidos, -1 pra erro.
int readHighScore(SCORE scores[MAXSCORES])
{
    FILE *arq;
    int result_leitura, i, iserr; // variavel para verificar se leitura deu certo

    if (!(arq = fopen(SCOREPATH, "rb"))) // abre para leitura e testa leitura
        return 0;
    else
    {
        result_leitura = fread(scores, sizeof(SCORE), MAXSCORES, arq);
        iserr = ferror(arq);
        fclose(arq);
    }
    if (result_leitura <= MAXSCORES && !iserr) // teste da leitura
        return result_leitura;

    return -1;
}

//escreve os highscores, retorna 1 pra sucesso. 
int writeHighScore(SCORE scores[MAXSCORES], int num_scores)
{
    FILE *arq;
    int result_escrita;

    if (!(arq = fopen(SCOREPATH, "wb")))
    { // abre para escrita binaria e testa a abertura
        printf("Erro na abertura do arquivo.");
        return 0;
    }
    else
    {
        result_escrita = fwrite(scores, sizeof(SCORE), num_scores, arq);
        fclose(arq);
        if (result_escrita != num_scores)
        { // testa se escreveu certo
            printf("Erro na escrita do arquivo.");
            return 0;
        }
    }

    return 1;
}

// retorna um score valido, a nao ser que o usuario saia da tela (retorna score com pontuacao -1)
SCORE getHighScore(int pontos)
{
    int curr_key, str_terminator_pos = 0;
    char curr;
    SCORE toReturn = {"", pontos};

    while (!WindowShouldClose())
    {
        curr = GetCharPressed();

        while (curr != 0)
        {
            if (str_terminator_pos >= 44)
                continue;

            toReturn.nome[str_terminator_pos] = curr;
            curr = GetCharPressed();
            str_terminator_pos++;
        }
        curr_key = GetKeyPressed();
        if (curr_key == KEY_ENTER)
            return toReturn;
        if (curr_key == KEY_BACKSPACE && str_terminator_pos > 0)
            str_terminator_pos--;

        toReturn.nome[str_terminator_pos] = '\0';

        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Escreva seu nome:", 40, 380, 44, WHITE);
            DrawRectangle(40, 440, 880, 60, WHITE);
            DrawRectangle(44, 444, 872, 52, BLACK);
            DrawText(toReturn.nome, 48, 448, 44, WHITE);
        EndDrawing();
    }

    return (SCORE){{0}, -1};
}

// retorna o indice (0-9) onde o Score se encontra. -1 se nao e highscore.
int isHighScore(int possible)
{
    SCORE scores[MAXSCORES];
    int result_leitura = readHighScore(scores), i;

    printf("res leitura: %d", result_leitura);
    if (result_leitura == -1)
        return -1;
    for (i = 0; i < result_leitura; i++)
    {
        if (possible > scores[i].pontos)
        {
            printf("is going to be inserted at: %d", i);
            return i;
        }
    }
    if (result_leitura < MAXSCORES)
        return result_leitura + 1;

    return -1;
}

//insere o highscore na posicao e empurra os outros pra baixo.
void insertNewScore(SCORE novo_score, int posicao)
{
    SCORE scores[MAXSCORES] = {0};
    int result_leitura = readHighScore(scores), i, last = -1;
    char vazio[45] = {0};

    if (result_leitura < 0)
        result_leitura = 0;

    int new_count = (result_leitura < MAXSCORES) ? (result_leitura + 1) : MAXSCORES;

    if (posicao < 0)
        posicao = 0;
    if (posicao > new_count - 1)
        posicao = new_count - 1;

    for (i = new_count - 1; i > posicao; i--)
    {
        scores[i] = scores[i - 1];
    }

    scores[posicao] = novo_score;

    writeHighScore(scores, result_leitura + 1);
}

//escreve highscores na tela
void showHighScores()
{
    SCORE scores[MAXSCORES] = {0};
    int result_leitura = readHighScore(scores), i, last = -1;
    char vazio[45] = {0};

    for (i = 0; i < MAXSCORES; i++)
    {
        if ((strcmp(scores[i].nome, vazio) == 0) && scores[i].pontos == 0)
        {
            last = i;
            break;
        }
    }

    if (last == -1)
        last = 10;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLUE);
        DrawText("Highscores:", 4, 4, 40, WHITE);
        for (i = 0; i < last; i++)
        {
            DrawText(scores[i].nome, 4, 50 * i + 50, 40, WHITE);
            DrawText(TextFormat("%d", scores[i].pontos), 850, 50 * i + 50, 40, WHITE);
        }
        EndDrawing();
    }
}