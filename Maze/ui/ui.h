#pragma once
#include "../settings/types.h"
#include "raylib.h"

void DrawSidebar(const Player* p, int screenW, int screenH, Font uiFont);
void DrawMenu(int screenW, int screenH, int sel, Font uiFont);
void DrawWin(int screenW, int screenH, Font uiFont);
void DrawPlayer(const Player* p);