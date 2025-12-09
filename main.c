#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "highscore.c"
#include <math.h>
#include "map.c"


#define MOVSPEED 2.0f
#define TAMNAVE 40
#define FPS 60

/*
Sprite: PosX, PosY, SizeX, Sizey
Plane left:43, 74, 40, 40
Plane regular:103, 70, 40, 40
Plane right:163, 74, 40, 40

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
    Vector2 hasCollided;
    
    const int screenWidth = 960, screenHeight = 840;
    int vidas, combustivel, nivel, score, naveState = 1, sideMovement = 0, run=1;
    float correction;
    char curr_map[20][24];
    loadMapTextFile("./Mapas/mapa03.txt", curr_map);
    Vector2 naveVel = {0,0};
    
    Vector2 navePos = ParseStartPoint(curr_map);

    Rectangle mapBounds = {0.0f, 40.0f, 960.0f, 840.0f};
    
    
    Rectangle naveSprite[] = {
        { 43.0f, 74.0f, 40.0f, 40.0f },
        { 103.0f, 70.0f, 40.0f, 40.0f },
        { 163.0f, 74.0f, 40.0f, 40.0f }
}; // Example: first 32x32 pixel frame

    InitWindow(screenWidth, screenHeight, "Nave Maluca - (C) 2025");
    Texture2D spriteSheet = LoadTexture("sprites.png");

    SetTargetFPS(FPS);
    


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        if(IsKeyDown(KEY_LEFT_SHIFT)) run = 3;
        else run=1;
        //pre drawing phase
        
        if((IsKeyDown(KEY_D)|| IsKeyDown(KEY_RIGHT))&& mapBounds.width >= navePos.x+TAMNAVE) {
            naveState = 2;
            naveVel.x += MOVSPEED * run;
            sideMovement = -1;
        } else if((IsKeyDown(KEY_A)|| IsKeyDown(KEY_LEFT)) && mapBounds.x  <= navePos.x){
            naveState = 0;
            naveVel.x -= MOVSPEED * run;
            sideMovement = 1;
        } else {
            naveState = 1;
            sideMovement = 0;
        }
        
        if((IsKeyDown(KEY_S)|| IsKeyDown(KEY_DOWN)) && mapBounds.height >= navePos.y + TAMNAVE){
            naveVel.y = MOVSPEED * run;

        } else if ((IsKeyDown(KEY_W)|| IsKeyDown(KEY_UP)) &&  mapBounds.y <= navePos.y){
            naveVel.y = -MOVSPEED * run;

        }
        
        

        naveVel = Vector2ClampValue(naveVel, MOVSPEED*run, MOVSPEED*run);
        
        hasCollided = testCollisionBad(navePos, naveVel, curr_map);

        navePos = Vector2Add(navePos, hasCollided);
        naveVel = Vector2Zero();
        
        //drawing phase
        BeginDrawing();
            ClearBackground(LIGHTGRAY);
            DrawMap(curr_map, spriteSheet);
            
            DrawTextureRec(spriteSheet, naveSprite[naveState], navePos,WHITE);
            DrawText(TextFormat("%f",ParseStartPoint(curr_map).x) , 1, 1, 38, BLACK);
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

