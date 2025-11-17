#include "raylib.h"
#include <stdio.h>
#include "highscore.c"
#include <math.h>
#include "map.c"

#define MOVSPEED 2.0f
#define TAMNAVE 56

/*
Sprite: PosX, PosY, SizeX, Sizey
Plane left:43, 74, 56, 56
Plane regular:103, 70, 56, 56
Plane right:163, 74, 56, 56

*/

int main(void){
    
    //Inicializa resolucao e fps.

    char test_map[20][24] = {
        "TTTTTTTTT       TTTTTTTT",
        "TTTTTTTT   X      TTTTTT",
        "TTTTTTT            TTTTT",
        "TTTTTT             TTTTT",
        "TTTTTT         X   TTTTT",
        "TTTTTTT            TTTTT",
        "TTTTTTTT         TTTTTTT",
        "TTTTTTTTT       TTTTTTTT",
        "TTTTTTTTT   T    TTTTTTT",
        "TTTTTTT     TTT   TTTTTT",
        "TTTTTT   X TTTTTT  TTTTT",
        "TTTTTT    TTTTTTT   TTTT",
        "TTTTT       TTTT     TTT",
        "TTTTT             X TTTT",
        "TTTTT               TTTT",
        "TTTTT       X       TTTT",
        "TTTTTT              TTTT",
        "TTTTTTT            TTTTT",
        "TTTTTTTTT         TTTTTT",
        "TTTTTTTTTT   A  TTTTTTTT"
    };
    
    const int screenWidth = 960, screenHeight = 840;
    int vidas, combustivel, nivel, score, naveState = 1, sideMovement = 0, run=1;
    float correction;
    char curr_map[20][24];
    
    Vector2 navePos = { (float)screenWidth/2, (float)screenHeight/2 };

    Rectangle mapBounds = {0.0f, 40.0f, 960.0f, 840.0f};
    
    
    Rectangle naveSprite[] = {
        { 43.0f, 74.0f, 56.0f, 56.0f },
        { 103.0f, 70.0f, 56.0f, 56.0f },
        { 163.0f, 74.0f, 56.0f, 56.0f }
}; // Example: first 32x32 pixel frame

    InitWindow(screenWidth, screenHeight, "Nave Maluca - (C) 2025");
    Texture2D spriteSheet = LoadTexture("sprites.png");

    SetTargetFPS(60);
    loadMapTextFile("./Mapas/mapa03.txt", curr_map);


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        if(IsKeyDown(KEY_LEFT_SHIFT)) run = 3;
        else run=1;
        //pre drawing phase
        if(IsKeyDown(KEY_D) && mapBounds.width >= navePos.x+TAMNAVE) {
            naveState = 2;
            navePos.x += MOVSPEED * run;
            sideMovement = -1;
        } else if(IsKeyDown(KEY_A) && mapBounds.x  <= navePos.x){
            naveState = 0;
            navePos.x -= MOVSPEED * run;
            sideMovement = 1;
        } else {
            naveState = 1;
            sideMovement = 0;
        }
        
        if(IsKeyDown(KEY_S) && mapBounds.height >= navePos.y + TAMNAVE){
            navePos.y += MOVSPEED * run;

        } else if (IsKeyDown(KEY_W) &&  mapBounds.y <= navePos.y){
            navePos.y -= MOVSPEED * run;

        }
        //drawing phase
        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawMap(test_map, spriteSheet);
            
            DrawTextureRec(spriteSheet, naveSprite[naveState], navePos,WHITE);
            DrawText(TextFormat("%f, %f",navePos.x, mapBounds.x), 1, 1, 38, BLACK);
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

