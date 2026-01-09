    #include "raylib.h"
    #include "settings/config.h"
    #include "settings/types.h"
    #include "logic/maze.h"
    #include "ui/ui.h"
    #include "logic/pathfinding.h"
    #include "logic/game.h"
    #include <time.h>
    #include <stdint.h>
    #include "ui/font_dejavu.h"
    #include "ui/icon_ico.h"
    
    #define UI_ICON     ui_icon
    #define UI_ICON_len ui_icon_len

    // --- независимый от raylib LCG для выбора варианта ---
    static uint32_t g_lcg_state = 0;

    static inline void InitLocalRNG(void) {
        uintptr_t mix = (uintptr_t)&g_lcg_state;
        g_lcg_state = (uint32_t)(time(NULL) ^ (mix >> 3) ^ 0x9E3779B9u);
        // один прогон, чтобы не быть равным нулю
        g_lcg_state = g_lcg_state * 1664525u + 1013904223u;
    }

    static inline uint32_t NextRand(void) {
        g_lcg_state = g_lcg_state * 1664525u + 1013904223u;
        return g_lcg_state;
    }

    // Возвращает случайный индекс 0..9, гарантированно отличный от cur (использует LCG выше)
    static inline int RandomVariantNotEqual(int cur) {
        int v = (int)(NextRand() % 10u);
        if (v == cur) v = (v + 1) % 10;
        return v;
    }

    int main(void){
        const int screenW = MAP_W*TILE + 240;
        const int screenH = MAP_H*TILE;

        SetConfigFlags(FLAG_VSYNC_HINT);
        InitWindow(screenW, screenH, "Maze Agent (C + raylib)");
        
        // иконка из памяти (ICO встроен в exe)
        Image iconImg = LoadImageFromMemory(
            ".ico",                 // формат
            UI_ICON,                // данные
            (int)UI_ICON_len        // размер
        );
        if (iconImg.data != NULL) {
            SetWindowIcon(iconImg);
            UnloadImage(iconImg);
        }
        
        SetTargetFPS(60);

        // инициализируем наш независимый RNG
        InitLocalRNG();

        // Load UI font with ASCII + Unicode arrows (← ↑ → ↓)
        int cps[95 + 4];
        for (int i = 0; i < 95; ++i) cps[i] = 32 + i; // ASCII 32..126
        cps[95] = 0x2190; // ←
        cps[96] = 0x2191; // ↑
        cps[97] = 0x2192; // →
        cps[98] = 0x2193; // ↓
        //Font uiFont = LoadFontEx("assets/DejaVuSans.ttf", 32, cps, 99);
        //SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
        Font uiFont = LoadFontFromMemory(
        ".ttf",                      // расширение для хинта
        DejaVuSans_ttf,              // данные
        (int)DejaVuSans_ttf_len,     // размер
        32,                          // кегль
        cps, 99                      // набор кодпоинтов
        );

    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

        GameState state = GS_MENU;
        int menuSel = 0;

        Maze maze = (Maze){0};
        Player pl  = (Player){0};

        unsigned seeds[10] = { 1337, 42, 7, 2025, 9001, 31415, 27182, 123456, 8080, 1001 };

        // ВАЖНО: первичный выбор варианта — тоже из локального RNG, не из raylib
        int currentVariant = (int)(NextRand() % 10u);
        StartNewRun(&maze, &pl, seeds[currentVariant]);

        while(!WindowShouldClose()){
            // --- Update ---
            switch(state){
                case GS_MENU:
                    if (IsKeyPressed(KEY_UP)   || IsKeyPressed(KEY_W))   menuSel = (menuSel+1)%2;
                    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))   menuSel = (menuSel+1)%2;
                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_KP_ENTER)){
                        if (menuSel==0){
                            // гарантия: новый вариант ≠ текущий, выбор через локальный RNG
                            currentVariant = RandomVariantNotEqual(currentVariant);
                            StartNewRun(&maze, &pl, seeds[currentVariant]);
                            state = GS_PLAY;
                        } else state = GS_EXIT;
                    }
                    break;

                case GS_PLAY: {
                    if (IsKeyPressed(KEY_ESCAPE)) state = GS_MENU;

                    int dx=0, dy=0;
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) dx=1;
                    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) dx=-1;
                    if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) dy=1;
                    if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) dy=-1;

                    if (dx||dy){
                        int nx=pl.pos.x+dx, ny=pl.pos.y+dy;
                        if (inBounds(nx,ny) && isWalkable(maze.grid[ny][nx])){
                            pl.pos.x=nx; pl.pos.y=ny;
                            if(!NextStepBFS(&maze, pl.pos, maze.exit, &pl.nextDir)){
                                pl.nextDir=(P2){0,0};
                            }
                        }
                    }

                    if (IsKeyPressed(KEY_R)){
                        // выбор нового варианта через независимый RNG → не зациклится на 2-х
                        currentVariant = RandomVariantNotEqual(currentVariant);
                        StartNewRun(&maze, &pl, seeds[currentVariant]);
                    }

                    if (pl.pos.x==maze.exit.x && pl.pos.y==maze.exit.y){
                        state = GS_WIN;
                    }
                } break;
                case GS_WIN:
                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)  || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_ESCAPE)){
                        state = GS_MENU;
                    }
                    break;

                case GS_EXIT: break;
            }

            // --- Draw ---
            BeginDrawing();
            ClearBackground(RAYWHITE);

            if (state==GS_MENU){
                DrawMenu(screenW, screenH, menuSel, uiFont);
            } else if (state==GS_PLAY){
                DrawMaze(&maze);
                DrawExit(&maze);
                DrawPlayer(&pl);
                DrawSidebar(&pl, screenW, screenH, uiFont);
            } else if (state==GS_WIN){
                DrawMaze(&maze);
                DrawExit(&maze);
                DrawPlayer(&pl);
                DrawSidebar(&pl, screenW, screenH, uiFont);
                DrawWin(screenW, screenH, uiFont);
            }

            EndDrawing();
            if (state==GS_EXIT) break;
        }

        UnloadFont(uiFont);
        CloseWindow();
        return 0;
    }