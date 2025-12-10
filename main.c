#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "highscore.c"
#include <math.h>
#include "map.c"

#define MAX_MAP 6
#define MOVSPEED 2.0f
#define TAMNAVE 40
#define FPS 60
#define VELBALA -4
/*
Sprite: PosX, PosY, SizeX, Sizey
Plane left:43, 74, 40, 40
Plane regular:103, 70, 40, 40
Plane right:163, 74, 40, 40

*/

/*
Um breve comentario
As funcoes definidas neste programa sao extremamente esmiucadas, mas por um
motivo muito bom, na programacao de jogos o que mais importa alem de tempo
de execucao e ordem de execucao, e pode parecer "bobo", mas um exercico na
decisao da ordem de execucao pode ser extremamente util! (nao mover inimigos
e testar colisao dos mesmos antes de mostrar pro player, por exemplo)

Claro, nesse caso, e muito rapido, apenas 1/60 de um segundo, mas e um
exercicio pra futuras questoes que dependem de ordem, porque nesse caso:

A ordem dos tratores ALTERA o viaduto!

Alem disos, comentarios gerais:
Funcoes pequenas serao pouquissimo comentadas (se comentadas), mas a depender
de sua obviedade para o leitor, e, em varios outros casos, elas nao terao
comentario inicial, apenas explicando o interior delas (quando a chaamada
for autoevidente, e.g. drawMenu)
*/

typedef struct
{
    Rectangle hitbox;
    Vector2 speed;
} BALA;

// Move as BALAs do array balas e colide-as com o mapa
void moveBala(BALA balas[20], char curr_map[20][24])
{
    int i;
    Vector2 resSpeed;

    for (i = 0; i < 20; i++)
    {
        // ignora balas nulas
        if (balas[i].speed.y == 0)
            continue;

        // testa colisao das balas
        resSpeed = testCollision(balas[i].hitbox, balas[i].speed, curr_map);

        // se ela colidir ou sair do mapa, zera a bala.
        if (!Vector2Equals(resSpeed, balas[i].speed) ||
            balas[i].hitbox.x + balas[i].speed.x <= 200)
        {
            balas[i] = (BALA){{0, 0, 0, 0}, {0, 0}};
        }
        else
        {
            balas[i].hitbox.y += balas[i].speed.y;
        }
    }
}

// testa se as BALAs atingem os postos de gasolina (e, caso atinjam, atualiza o mapa pra tirar o posto)
// retorna a quantidade de gasolina que o player deve receber.
int testBulletGas(Rectangle postos[20], BALA balas[20], Sound posto_kabum, char curr_map[20][24])
{
    int gasolina = 0, i, j, k;
    Rectangle thitbox;

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 20; j++)
        {
            // se bala ou posto sao nulas, ignora.
            if (postos[i].height == 0)
                continue;
            if (balas[i].hitbox.height == 0)
                continue;

            // corrige pro header.
            thitbox = postos[i];
            thitbox.y += 40;

            // se nao colide, pula
            if (!CheckCollisionRecs(thitbox, balas[j].hitbox))
                continue;

            // se colide, remove o posto do mapa, zera a bala e adiciona gasolina
            gasolina += 20;
            for (k = 0; k < 4; k++)
            {
                curr_map[(int)postos[i].y / 40][(int)(postos[i].x - 40) / 40 + k] = ' ';
            }
            balas[j] = (BALA){{0, 0, 0, 0}, {0, 0}};
            PlaySound(posto_kabum);
        }
    }

    return gasolina;
}

// testa se as balas atingem a(s) pontes. retorna o score a ser adicionado.
int testBulletBridge(char curr_map[20][24], BALA balas[20], Sound bridge_explosion)
{
    int i, j, bridgecol, score = 0;

    for (i = 0; i < 20; i++)
    {
        // remove balas nulas
        if (balas[i].speed.y == 0)
            continue;

        // remove a ponte na alutra bridgecol (0-19)
        bridgecol = TestCollisionBridges(curr_map, balas[i].hitbox);
        if (bridgecol != -1)
        {
            for (j = 0; j < 24; j++)
            {
                if (curr_map[bridgecol][j] == 'P')
                    curr_map[bridgecol][j] = ' ';
            }
            PlaySound(bridge_explosion);
            score += 200;
            balas[i] = (BALA){{0, 0, 0, 0}, {0, 0}};
        };
    }
    return score;
}

// move todos os inimigos e, caso colidam, troca a velocidade no eixo X e Y
void moveEnemies(ENEMY enemies[20], char curr_map[20][24])
{
    int i;
    Vector2 resSpeed;

    for (i = 0; i < 20; i++)
    {
        if (!enemies[i].points)
            continue;
        resSpeed = testCollision(enemies[i].hitbox, enemies[i].speed, curr_map);
        if (!Vector2Equals(resSpeed, enemies[i].speed))
        {
            enemies[i].speed.x *= -1;
            enemies[i].speed.y *= -1;
        }
        enemies[i].hitbox.x += enemies[i].speed.x;
        enemies[i].hitbox.y += enemies[i].speed.y;
    }
}

// checa se os inimigos colidiram com o hitbox Player, retorna qual dos inimigos a hitbox colidiu
int checkEnemyCollision(ENEMY enemies[20], Rectangle player)
{
    int i;

    for (i = 0; i < 20; i++)
    {
        // pula inimigos nulos
        if (enemies[i].points == 0)
            continue;
        if (CheckCollisionRecs(enemies[i].hitbox, player))
            return i;
    }
    return -1;
}

// Desenha o menu de pausa na tela
void drawMenuPausa(int option)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Deseja sair?", 300, 300, 50, WHITE);
    DrawText("NAO", 400, 400, 40, WHITE);
    if(!option)
    {
        DrawText("SIM", 400, 460, 40, WHITE);
        DrawRectangle(350, 405, 30, 30, YELLOW);
    }
    else
    {
        DrawText("TEM CERTEZA? :c", 320, 460, 40, WHITE);
        DrawRectangle(270, 465, 30, 30, YELLOW);
    }
    
    EndDrawing();
}

// loop principal, roda quando o player pressiona "jogar".
void levelLoop(Texture2D spriteSheet)
{

    // das variaveis nao auto_evidentes:
    /*
    naveState - faz com que o sprite da nave mude
    frametimer - serve de estado global para coisas que rodam uma, duas ou mais vezes por segundo, mas nao rodam uma vez por frame
    contbala - quantas balas ja foram atiradas. reseta no 20.
    cflag - se o player deseja sair do mapa, forma de propagar o estado de saida para mais de um loop.
    restart - serve pra decidir se deve reiniciar o score/vidas/combustivel do player (pro inicio do nivel)
    breakpausa - quebra o lock do menu de pausa
    */
    int vidas = 3, combustivel = 100, nivel = 1, score = 0, naveState = 1, run = 1, i, frametimer = FPS,
        contbala, j, cflag = 0, hspos, old_vidas, old_combustivel, old_score, restart = 0, breakpausa, opcao_pausa = 0;

    char curr_map[20][24];
    BALA balas[20] = {0};
    ENEMY enemies[20] = {0};
    Rectangle postos[20] = {0};
    SCORE hs;

    Sound simple_explosion = LoadSound("atari_boom3.wav");
    Sound player_death = LoadSound("atari_boom5.wav");
    Sound bridge_explosion = LoadSound("atari_boom2.wav");

    Vector2 naveVel = {0, 0};
    Vector2 navePos;

    // contem o vetor de velocidade resultante para nao colidir com a parede (ir ate ela, mas nao mais)
    Vector2 hasCollided;

    Rectangle mapBounds = {0.0f, 40.0f, 960.0f, 840.0f};

    Rectangle naveSprite[] = {
        {43.0f, 74.0f, 40.0f, 40.0f},
        {103.0f, 70.0f, 40.0f, 40.0f},
        {163.0f, 74.0f, 40.0f, 40.0f}};

    while (cflag != 1 && nivel < 6)
    {
        // se o player reinicia, reseta as variaveis pras do inicio do ultimo nivel.
        if (restart)
        {
            score = old_score;
            combustivel = old_combustivel;
            vidas = old_vidas;
            restart = 0;
        }
        else
        {
            old_score = score;
            old_combustivel = combustivel;
            old_vidas = vidas;
        }

        // zera os inimigos, postos e balas.
        for (i = 0; i < 20; i++)
        {
            balas[i] = (BALA){{0, 0, 0, 0}, {0, 0}};
            enemies[i] = (ENEMY){{0, 0, 0, 0}, {0, 0}, {0, 0, 0, 0}, 0};
            postos[i] = (Rectangle){0, 0, 0, 0};
        }
        contbala = 0;

        // Carrega mapas, inimigos, postos e posicao da nave.
        loadMapTextFile(TextFormat("./Mapas/mapa%02d.txt", nivel), curr_map);
        parseEnemies(enemies, curr_map);
        parsePostos(postos, curr_map);
        navePos = ParseStartPoint(curr_map);

        while (cflag != 1 && navePos.y > 40)
        {

            // permite andar mais rapido com o shift
            if (IsKeyDown(KEY_LEFT_SHIFT))
                run = 3;
            else
                run = 1;

            // decrementa combustivel uma vez por segundo
            if (frametimer == 0)
            {
                combustivel--;
            }

            // se alguma das balas colidir com um inimigo, zera ele e a bala e adiciona ao score
            for (i = 0; i < 20; i++)
            {
                j = checkEnemyCollision(enemies, balas[i].hitbox);
                if (j != -1)
                {
                    score += enemies[j].points;
                    enemies[j] = (ENEMY){{0, 0, 0, 0}, {0, 0}, {0, 0, 0, 0}, 0};
                    PlaySound(simple_explosion);
                    balas[i] = (BALA){{0, 0, 0, 0}, {0, 0}};
                }
            }

            // colisao com pontes
            score += testBulletBridge(curr_map, balas, bridge_explosion);

            // colisao com postos e clamp do valor do combustivel
            combustivel += testBulletGas(postos, balas, bridge_explosion, curr_map);
            if (combustivel > 100)
                combustivel = 100;

            // mata o player se ele colide com os inimigos
            if (checkEnemyCollision(enemies, (Rectangle){navePos.x, navePos.y, 40, 40}) != -1)
            {
                PlaySound(player_death);
                vidas--;
                navePos = ParseStartPoint(curr_map);
            }

            // codigo de moviemntacao, tambem atualiza o sprite da nave.
            if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && mapBounds.width >= navePos.x + TAMNAVE)
            {
                naveState = 2;
                naveVel.x += MOVSPEED * run;
            }
            else if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && mapBounds.x <= navePos.x)
            {
                naveState = 0;
                naveVel.x -= MOVSPEED * run;
            }
            else
            {
                naveState = 1;
            }

            if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && mapBounds.height >= navePos.y + TAMNAVE)
            {
                naveVel.y = MOVSPEED * run;
            }
            else if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && mapBounds.y <= navePos.y)
            {
                naveVel.y = -MOVSPEED * run;
            }

            // atira
            if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_SPACE))
            {
                balas[contbala] = (BALA){
                    {navePos.x + 5, navePos.y + 15, 10, 15},
                    {0, VELBALA}};

                if (contbala < 19)
                    contbala++;
                else
                    contbala = 0;
            }

            moveBala(balas, curr_map);

            // colisao da nave com o cenario
            naveVel = Vector2ClampValue(naveVel, MOVSPEED * run, MOVSPEED * run);
            Rectangle a = {navePos.x, navePos.y, 40, 40};
            hasCollided = testCollision(a, naveVel, curr_map);

            navePos = Vector2Add(navePos, hasCollided);
            naveVel = Vector2Zero();

            // reseta o frametimer
            if (frametimer <= 0)
                frametimer = FPS;
            frametimer--;

            // move os inimigos
            moveEnemies(enemies, curr_map);

            // codigo que desenha na tela
            BeginDrawing();
            // desenha mapa e background
            ClearBackground(LIGHTGRAY);
            DrawMap(curr_map, spriteSheet);

            // desenha nave
            DrawTextureRec(spriteSheet, naveSprite[naveState], navePos, WHITE);

            for (i = 0; i < 20; i++)
            {
                // desenha inimigos
                if (enemies[i].points == 30)
                {
                    // gira a helice do helicoptero
                    if (frametimer <= 30)
                    {
                        DrawTextureRec(spriteSheet, (Rectangle){83, 186, 40, 25}, (Vector2){enemies[i].hitbox.x, enemies[i].hitbox.y}, WHITE);
                    }
                    else
                        DrawTextureRec(spriteSheet, enemies[i].sprite, (Vector2){enemies[i].hitbox.x, enemies[i].hitbox.y}, WHITE);
                }
                else if (enemies[i].points != 0)
                {
                    // desenha o resto dos inimigos (nesse caso, so navios)
                    DrawTexturePro(spriteSheet, enemies[i].sprite, enemies[i].hitbox, (Vector2){0, 0}, 0, WHITE);
                }

                // desenha balas
                if (balas[i].speed.y != 0)
                {
                    DrawRectangleRec(balas[i].hitbox, YELLOW);
                }
            }

            // desenha pontes
            DrawMapBridges(curr_map, spriteSheet);

            // desenha interface
            DrawText(TextFormat("Vidas: %d  Score: %d  Combustivel: %d  %f", vidas, score, combustivel, navePos.y), 1, 1, 38, BLACK);
            EndDrawing();

            // caso a nave tenha ultrapassado a linha pro proximo nivel, passa de nivel
            if (navePos.y <= 40)
            {
                nivel++;
            }

            if (WindowShouldClose() || IsKeyPressed(KEY_P))
            {
                breakpausa = 0;
                opcao_pausa = 0;
                while (!breakpausa)
                {
                    drawMenuPausa(opcao_pausa);
                    if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && opcao_pausa < 1)
                    {
                        opcao_pausa++;
                    }
                    if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && opcao_pausa > 0)
                    {
                        opcao_pausa--;
                    }
                    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
                        if(opcao_pausa){
                            breakpausa = 1;
                            cflag = 1;
                        }
                        else
                            breakpausa = 1;
                    }
                    if(WindowShouldClose()){
                        breakpausa = 1;
                    }
                }
            }

            // forca reset (sem aumentar o nivel e mantendo score/gasolina/etc)
            if (IsKeyPressed(KEY_R))
            {
                navePos.y = -40;
                restart = 1;
            }

            // passa pro highscore se o usuario aperta esc, acaba o combustivel ou vidas
            if (combustivel <= 0 || vidas < 0)
            {
                cflag = 1;
            }
        }
    }

    // insere o highscore na posicao hspos
    hspos = isHighScore(score);
    if (hspos != -1)
    {
        hs = getHighScore(score);
        if (hs.pontos == score)
        {
            insertNewScore(hs, hspos);
        }
    }
}

// desenha o menu de opcoes
void drawMenu(int option)
{

    BeginDrawing();
    ClearBackground(BLUE);
    DrawText("Jogar", 250, 150, 80, WHITE);
    DrawText("Ranking", 250, 350, 80, WHITE);
    DrawText("Sair", 250, 550, 80, WHITE);
    DrawRectangle(160, (option * 200 + 170), 40, 40, YELLOW);
    EndDrawing();
}

int main(void)
{

    // Inicializa resolucao e fps.
    const int screenWidth = 960, screenHeight = 840;
    InitWindow(screenWidth, screenHeight, "Nave Maluca - (C) 2025");
    SetTargetFPS(FPS);
    InitAudioDevice();

    // carrega sprites e inicializa variaveis
    Texture2D spriteSheet = LoadTexture("sprites.png");
    int menuPos = 0;

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // codigo de logica do menu
        if (IsKeyPressed(KEY_ENTER))
        {
            switch (menuPos)
            {
            case 0:
                levelLoop(spriteSheet);
                break;
            case 1:
                showHighScores();
                break;
            default:
                CloseWindow();
                return 0;
            }
        }

        // movimentacao dentro do menu
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && menuPos < 2)
        {
            menuPos++;
        }
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && menuPos > 0)
        {
            menuPos--;
        }

        drawMenu(menuPos);
    }

    // de-inicializa audio e video
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
