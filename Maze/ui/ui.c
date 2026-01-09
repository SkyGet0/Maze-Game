#include "ui.h"
#include <stdio.h>
#include "raylib.h"
#include "../logic/pathfinding.h"
#include "config.h"

// map direction -> Unicode arrow
static const char* ArrowUtf8(P2 d){
    if      (d.x== 1 && d.y== 0) return "→";
    else if (d.x==-1 && d.y== 0) return "←";
    else if (d.x== 0 && d.y== 1) return "↓";
    else if (d.x== 0 && d.y==-1) return "↑";
    return "·";
}

void DrawPlayer(const Player* p){
    Rectangle r={ p->pos.x*TILE, p->pos.y*TILE, TILE, TILE };
    DrawRectangleRec(r, (Color){60,180,75,255});
    DrawRectangleLinesEx(r, 2, (Color){0,90,0,255});
}

// helper: draw centered text (pure C)
static void DrawCenteredText(Font font, const char* txt,
                             float centerX, float y,
                             float fontSize, float spacing, Color col)
{
    Vector2 sz = MeasureTextEx(font, txt, fontSize, spacing);
    DrawTextEx(font, txt, (Vector2){ centerX - sz.x*0.5f, y }, fontSize, spacing, col);
}

void DrawSidebar(const Player* p, int screenW, int screenH, Font uiFont){
    const float sideX  = (float)(MAP_W * TILE);
    const float panelW = (float)screenW - sideX;
    const float padX   = 16.0f;
    const float cx     = sideX + panelW * 0.5f; // center X

    // sidebar background + separator
    DrawRectangle((int)sideX, 0, (int)panelW, screenH, (Color){245,245,250,255});
    DrawLine((int)sideX, 0, (int)sideX, screenH, (Color){200,200,210,255});

    // inner card with soft corners
    Rectangle card = (Rectangle){ sideX + 8, 10, panelW - 16, (float)screenH - 20 };
    DrawRectangleRounded(card, 0.06f, 8, (Color){255,255,255,245});
    // your raylib has no thickness arg here:
    DrawRectangleRoundedLines(card, 0.06f, 8, (Color){220,220,230,255});

    float y = card.y + 16;

    // headings
    DrawCenteredText(uiFont, "Hint:",     cx, y, 26, 1, BLACK);                  y += 32;
    DrawCenteredText(uiFont, "Next step", cx, y, 20, 1, (Color){110,110,120,255}); y += 36;

    // big Unicode arrow
    const char* arrow = ArrowUtf8(p->nextDir);
    DrawCenteredText(uiFont, arrow, cx, y, 72, 2, BLACK);                        y += 72 + 12;

    // textual direction
    char buf[64];
    snprintf(buf, sizeof(buf), "Direction: %s",
        (p->nextDir.x== 1 && p->nextDir.y== 0) ? "Right" :
        (p->nextDir.x==-1 && p->nextDir.y== 0) ? "Left"  :
        (p->nextDir.x== 0 && p->nextDir.y== 1) ? "Down"  :
        (p->nextDir.x== 0 && p->nextDir.y==-1) ? "Up"    : "None");
    DrawCenteredText(uiFont, buf, cx, y, 20, 1, BLACK);                           y += 34;

    // divider
    DrawLine((int)(card.x + padX), (int)y, (int)(card.x + card.width - padX), (int)y,
             (Color){220,220,230,255});
    y += 16;

    // controls
    DrawCenteredText(uiFont, "Controls:", cx, y, 22, 1, BLACK);                  y += 30;

    const char* lines[] = {
        "Arrows / WASD = move",
        "R = new maze",
        "ESC = menu",
        "Enter = start"
    };
    for (int i=0; i<4; ++i){
        DrawCenteredText(uiFont, lines[i], cx, y, 18, 1, (Color){110,110,120,255});
        y += 24;
    }
}

void DrawMenu(int screenW, int screenH, int sel, Font uiFont){
    DrawRectangle(0,0,screenH,screenH,(Color){18,18,28,255}); // small fix: fill full screenW
    DrawRectangle(0,0,screenW,screenH,(Color){18,18,28,255});

    const char* title = "Maze Agent";
    Vector2 tsz = MeasureTextEx(uiFont, title, 36, 1);
    DrawTextEx(uiFont, title, (Vector2){(screenW-tsz.x)/2.0f, 120}, 36, 1, RAYWHITE);

    const char* items[2] = {"Start","Quit"};
    for(int i=0;i<2;++i){
        Vector2 sz = MeasureTextEx(uiFont, items[i], 28, 1);
        float x=(screenW-sz.x)/2.0f, y=220+i*50;
        Color c = (i==sel)? (Color){255,215,0,255} : RAYWHITE;
        DrawTextEx(uiFont, items[i], (Vector2){x,y}, 28, 1, c);
    }
}

void DrawWin(int screenW, int screenH, Font uiFont){
    DrawRectangle(0,0,screenW,screenH,(Color){18,18,28,240});
    const char* t1="You reached the exit!";
    const char* t2="Press Enter for Menu";
    Vector2 s1=MeasureTextEx(uiFont,t1,32,1);
    Vector2 s2=MeasureTextEx(uiFont,t2,22,1);
    DrawTextEx(uiFont,t1,(Vector2){(screenW-s1.x)/2.0f,140},32,1,RAYWHITE);
    DrawTextEx(uiFont,t2,(Vector2){(screenW-s2.x)/2.0f,190},22,1,(Color){255,215,0,255});
}