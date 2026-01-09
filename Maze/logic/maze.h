#pragma once
#include <stdbool.h>
#include "../settings/types.h"

bool inBounds(int x, int y);
bool isWalkable(char c);

void GenerateMaze(Maze* m, unsigned seed);

void DrawMaze(const Maze* m);
void DrawExit(const Maze* m);