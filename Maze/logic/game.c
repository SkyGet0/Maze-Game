#include "game.h"
#include "maze.h"
#include "pathfinding.h"
#include "raylib.h"

void StartNewRun(Maze* maze, Player* pl, unsigned variantSeed){
    SetRandomSeed(variantSeed);     // детерминированная вариация
    GenerateMaze(maze, variantSeed);
    pl->pos = maze->start;
    if(!NextStepBFS(maze, pl->pos, maze->exit, &pl->nextDir)){
        pl->nextDir=(P2){0,0};
    }
}