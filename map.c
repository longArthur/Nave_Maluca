#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include "raymath.h"

#define MAPSQUARE 40

//TODO: fix this function! 
void loadMapTextFile(const char *path, char matrix[20][24]) {
    FILE *file = fopen(path, "r");
    int i, j;
    char c;
    if (file == NULL) {
        printf("Failed to open file: %s\n", path);
        return;
    }

    for ( i = 0; i < 20; i++) {
        j = 0;
        c = 0;
        for (; j < 24; j++) {
            c = fgetc(file);
            if (c == EOF || c == '\n') break;
            matrix[i][j] = (char)c;
        }
        // Fill the rest with spaces if line is short or EOF
        for (; j < 24; j++) {
            matrix[i][j] = ' ';
        }
        // Skip to end of line if not already there
        while (c != '\n' && c != EOF) c = fgetc(file);
    }

    fclose(file);
}

Vector2 ParseStartPoint(char mapArray[20][24]){
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 24; j++) {
            if (mapArray[i][j] == 'A') {
                Vector2 returner = {j * 40, i * 40 + 40};
                return returner;
            }
        }
    }
    // If not found, return a default value (e.g., top-left corner)
    Vector2 returner = {0, 40};
    return returner;
}

void DrawMap(char mapArray[20][24], Texture spriteSheet){
    int i, j;
    Vector2 toDraw;
    //TODO: fix water
    Rectangle terrainTypes[] = {
        {340, 80, 40, 40},
        {220, 2, 40, 40}
    };

    for(i = 0; i < 20; i++){
        for(j = 0; j < 24; j++){
            toDraw.y = 40.0f*i + 40;
            toDraw.x = 40.0f*j; 
            switch(mapArray[i][j]){//TODO: add terrain types!
                case 'T':
                    DrawTextureRec(spriteSheet, terrainTypes[0],toDraw, WHITE);
                    break;
                default:
                    DrawTextureRec(spriteSheet, terrainTypes[1],toDraw, WHITE);
            }
        }
    }
}


int testCollision(Vector2 navePos,Vector2 naveVel, char mapArray[20][24]){
    int i, j;
    Vector2 navePosFile = {navePos.x/40, navePos.y/40}, naveMov = Vector2Add(navePos, naveVel);
    


    for(i = navePosFile.x - 2; i < navePosFile.x + 2; i++){
        for(j = navePosFile.y - 2; j < navePosFile.y + 2; j++){
            if(mapArray[i][j] == 't') {
                if(CheckCollisionRecs((Rectangle) {i*40, j*40, (i+1)*40, (j+1)*40},
                    (Rectangle) {naveMov.x, naveMov.y, naveMov.x+40, naveMov.y+40})) 
                    return 1;
            }
        }
    }

    return 0;
}



// Returns the maximum velocity vector without colliding with terrain.
// If a collision would occur, the velocity is reduced to stop at the collision boundary.
Vector2 testCollisionBad(Vector2 navePos, Vector2 naveVel, char mapArray[20][24]) {
    Vector2 resultVel = naveVel;
    Rectangle naveRect = { navePos.x, navePos.y - 40, 40, 40 }; // Adjust for header offset

    // Check X axis movement
    if (naveVel.x != 0) {
        Rectangle testRect = naveRect;
        testRect.x += naveVel.x;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 24; j++) {
                if (mapArray[i][j] == 'T') {
                    Rectangle terrainRect = { j * 40, i * 40, 40, 40 };
                    if (CheckCollisionRecs(testRect, terrainRect)) {
                        if (naveVel.x > 0)
                            resultVel.x = terrainRect.x - (naveRect.x + naveRect.width);
                        else
                            resultVel.x = (terrainRect.x + terrainRect.width) - naveRect.x;
                        // Clamp to zero if overshoot
                        if ((naveVel.x > 0 && resultVel.x < 0) || (naveVel.x < 0 && resultVel.x > 0))
                            resultVel.x = 0;
                    }
                }
            }
        }
    }

    // Check Y axis movement
    if (naveVel.y != 0) {
        Rectangle testRect = naveRect;
        testRect.y += naveVel.y;
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 24; j++) {
                if (mapArray[i][j] == 'T') {
                    Rectangle terrainRect = { j * 40, i * 40, 40, 40 };
                    if (CheckCollisionRecs(testRect, terrainRect)) {
                        if (naveVel.y > 0)
                            resultVel.y = terrainRect.y - (naveRect.y + naveRect.height);
                        else
                            resultVel.y = (terrainRect.y + terrainRect.height) - naveRect.y;
                        // Clamp to zero if overshoot
                        if ((naveVel.y > 0 && resultVel.y < 0) || (naveVel.y < 0 && resultVel.y > 0))
                            resultVel.y = 0;
                    }
                }
            }
        }
    }

    return resultVel;
}
