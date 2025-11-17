#include <stdio.h>
#include "raylib.h"
#include <string.h>

//TODO: fix this function! 
void loadMapTextFile(const char *path, char matrix[20][24]) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", path);
        return;
    }

    for (int i = 0; i < 20 && fgets(matrix[i], 25, file); i++) {
        // Remove newline character from the end of each line
        matrix[i][strcspn(matrix[i], "\n")] = '\0';
    }
}

Vector2 ParseStartPoint(char mapArray[20][24]){
    int i = 0;
    do{
        i++;
    } while(mapArray[i%24][i/24] != 'A');

    Vector2 returner = {i%24, i/24};
    return returner;
}

void DrawMap(char mapArray[20][24], Texture spriteSheet){
    int i, j;
    Vector2 toDraw;
    //TODO: fix water
    Rectangle terrainTypes[] = {
        {340, 80, 40, 40},
        {220, 1, 40, 40}
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