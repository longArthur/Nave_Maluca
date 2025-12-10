#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include "raymath.h"

#define MAPSQUARE 40
#define BOAT_SPEED 0
#define HELICOPTER_SPEED 2

typedef struct
{
    Rectangle hitbox;
    Vector2 speed;
    Rectangle sprite;
    int points;
} ENEMY;

//carrega mapa de um arquivo de texto
void loadMapTextFile(const char *path, char matrix[20][24])
{
    FILE *file = fopen(path, "r");
    int i, j;
    char c;

    if (file == NULL)
    {
        printf("Arquivo com erro: %s\n", path);
        return;
    }

    //carrega caractere-a-caractere o mapa
    //o codigo e antigo
    //nao vou mudar
    //fiz antes de aprender a ler arquivo direito
    for (i = 0; i < 20; i++)
    {
        j = 0;
        c = 0;
        for (; j < 24; j++)
        {
            c = fgetc(file);
            if (c == EOF || c == '\n')
                break;
            matrix[i][j] = (char)c;
        }
        // preenche espacos faltantes com ' ' (eof, fim de linha)
        for (; j < 24; j++)
        {
            matrix[i][j] = ' ';
        }

        while (c != '\n' && c != EOF)
            c = fgetc(file);
    }

    fclose(file);
}


//acha onde o player deve comecar
Vector2 ParseStartPoint(char mapArray[20][24])
{
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            if (mapArray[i][j] == 'A')
            {
                Vector2 returner = {j * 40, i * 40 + 40};
                return returner;
            }
        }
    }
    // se nao encontrado, escolhe o topo esquerdo como inicial
    Vector2 returner = {0, 40};
    return returner;
}

//testa a colisao do collider com as pontes, retorna a altura da ponte
int TestCollisionBridges(char mapArray[20][24], Rectangle collider)
{
    int i, j;
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 24; j++)
        {
            if (mapArray[i][j] == 'P' &&
                (collider.y <= i * 40 + 80 && collider.y >= i * 40 + 40))
            {
                return i;
            }
        }
    }
    return -1;
}

//desenha o mapa
void DrawMap(char mapArray[20][24], Texture spriteSheet)
{
    int i, j;
    Vector2 toDraw;

    //coordenadas no arquivo de sprites de cada tipo de terreno
    Rectangle terrainTypes[] = {
        {340, 80, 40, 40},
        {220, 2, 40, 40},
        {606, 163, 160, 40}
    };

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 24; j++)
        {
            //posicao a desenhar
            toDraw.y = 40.0f * i + 40;
            toDraw.x = 40.0f * j;

            switch (mapArray[i][j])
            { 
                case 'T':
                    DrawTextureRec(spriteSheet, terrainTypes[0], toDraw, WHITE);
                    break;
                case 'G':
                    //pula 3 pra contar com a lateralidade do posto de gasolina
                    DrawTextureRec(spriteSheet, terrainTypes[2], toDraw, WHITE);
                    j += 3;
                    break;
                default:
                    DrawTextureRec(spriteSheet, terrainTypes[1], toDraw, WHITE);
            }
        }
    }
}

//desenha as pontes no mapa, e separado pois ela precisa ser desenhada depois do player
void DrawMapBridges(char mapArray[20][24], Texture spritesheet)
{
    int i, j;

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 24; j++)
        {

            if (mapArray[i][j] == 'P')
                DrawTextureRec(spritesheet, (Rectangle){686, 65, 40, 40}, (Vector2){40 * j, 40 * i + 40}, WHITE);
        }
    }
}

//encontra os postos no arquivo e escreve eles no array postos[20]
void parsePostos(Rectangle postos[20], char mapArray[20][24])
{
    int i, j, k = 0;
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 24; j++)
        {
            if (mapArray[i][j] == 'G')
            {
                postos[k] = (Rectangle){j * 40 + 40, i * 40, 160, 40};
                j += 3;
                k++;
            }
        }
    }
}


//encontra os inimigos no mapArray
void parseEnemies(ENEMY array[20], char mapArray[20][24])
{

    int i, j, arri = 0;
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 24; j++)
        {

            switch (mapArray[i][j])
            {
            
                case 'X':
                    array[arri] = (ENEMY){

                        {j * MAPSQUARE, i * MAPSQUARE + 40, 40, 25},
                        {HELICOPTER_SPEED, 0},
                        {11, 186, 40, 25},
                        30};
                    arri++;
                    break;
                case 'N':
                    array[arri] = (ENEMY){
                        {j * MAPSQUARE, i * MAPSQUARE + 40, 80, 20},
                        {BOAT_SPEED, 0},
                        {15, 234, 80, 20},
                        60};
                    arri++;
                    break;
            }
        }
    }
}


//testa colisoes entre o Collisor no proximo frame se collisorVel for aplicada
//retorna o vetor maximo sem colisao.
Vector2 testCollision(Rectangle collisor, Vector2 collisorVel, char mapArray[20][24])
{
    collisor.y -= 40;
    Vector2 resultVel = collisorVel;

    // Checa o eixo X
    if (collisorVel.x != 0)
    {
        Rectangle testRect = collisor;
        testRect.x += collisorVel.x;
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 24; j++)
            {
                if (mapArray[i][j] == 'T')
                {
                    Rectangle terrainRect = {j * 40, i * 40, 40, 40};
                    if (CheckCollisionRecs(testRect, terrainRect))
                    {
                        if (collisorVel.x > 0)
                            resultVel.x = terrainRect.x - (collisor.x + collisor.width);
                        else
                            resultVel.x = (terrainRect.x + terrainRect.width) - collisor.x;
                        if ((collisorVel.x > 0 && resultVel.x < 0) || (collisorVel.x < 0 && resultVel.x > 0))
                            resultVel.x = 0;
                    }
                }
            }
        }
    }

    // checa o eixo y
    if (collisorVel.y != 0)
    {
        Rectangle testRect = collisor;
        testRect.y += collisorVel.y;
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 24; j++)
            {
                if (mapArray[i][j] == 'T')
                {
                    Rectangle terrainRect = {j * 40, i * 40, 40, 40};
                    if (CheckCollisionRecs(testRect, terrainRect))
                    {
                        if (collisorVel.y > 0)
                            resultVel.y = terrainRect.y - (collisor.y + collisor.height);
                        else
                            resultVel.y = (terrainRect.y + terrainRect.height) - collisor.y;
                        if ((collisorVel.y > 0 && resultVel.y < 0) || (collisorVel.y < 0 && resultVel.y > 0))
                            resultVel.y = 0;
                    }
                }
            }
        }
    }

    return resultVel;
}
