#include "../hFiles/score.h"
#include <cstdio>


int LoadHighScore(void){
    int hi = 0;

    FILE *file = fopen("highscore.txt","r");
    if(file!=NULL){
        fscanf(file,"%d",&hi);
        fclose(file);
    }
    return hi;
}

void SaveHighScore(int hiScore){
    FILE *file = fopen("highscore.txt","w");
    if(file!=NULL){
        fprintf(file,"%d",hiScore);
        fclose(file);
    }
}