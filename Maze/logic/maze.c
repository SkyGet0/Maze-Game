#include "maze.h"
#include "raylib.h"

static void FillWalls(Maze* m){
    for(int y = 0; y < MAP_H; ++y)
        for(int x = 0; x < MAP_W; ++x)
            m -> grid[y][x] = '#';
}

bool inBounds(int x, int y){ 
    return x >= 0 && x <MAP_W && y >= 0 && y < MAP_H; 
}

bool isWalkable(char c){ 
    return c == '.'; 
}

// DFS backtracker (perfect maze)
static void Carve(Maze* m, int x, int y){
    int dirs[4][2] = {{2, 0}, {-2 ,0}, {0, 2}, {0, -2}};
    // shuffle
    for (int i = 0; i < 4; i++){
        int j = GetRandomValue(i,3);
        int tx = dirs[i][0], ty = dirs[i][1];
        dirs[i][0] = dirs[j][0]; dirs[i][1] = dirs[j][1];
        dirs[j][0] = tx;         
        dirs[j][1] = ty;
    }
    for (int i = 0; i < 4; i++){
        int nx = x + dirs[i][0], ny = y + dirs[i][1];
        if (inBounds(nx, ny) && m -> grid[ny][nx] == '#'){
            int wx = x + dirs[i][0] / 2, wy = y + dirs[i][1] / 2;
            m -> grid[wy][wx]='.';
            m -> grid[ny][nx]='.';
            Carve(m, nx, ny);
        }
    }
}

void GenerateMaze(Maze* m, unsigned seed){
    (void)seed; // сид берём из SetRandomSeed снаружи
    m->w=MAP_W; m->h=MAP_H;
    FillWalls(m);

    int sx=1, sy=1;
    m->grid[sy][sx]='.';
    Carve(m, sx, sy);

    // рамка стен
    for(int x=0;x<MAP_W;x++){ m->grid[0][x]='#'; m->grid[MAP_H-1][x]='#'; }
    for(int y=0;y<MAP_H;y++){ m->grid[y][0]='#'; m->grid[y][MAP_W-1]='#'; }

    // Start слева, Exit справа по ближайшим проходам
    int syc=1; for(; syc<MAP_H-1; syc+=2) if (m->grid[syc][1]=='.') break;
    int eyc=MAP_H-2; for(; eyc>1; eyc-=2) if (m->grid[eyc][MAP_W-2]=='.') break;
    m->start=(P2){1, syc};
    m->exit =(P2){MAP_W-2, eyc};
}

void DrawMaze(const Maze* m){
    for(int y=0;y<m->h;++y){
        for(int x=0;x<m->w;++x){
            Rectangle r={ x*TILE, y*TILE, TILE, TILE };
            Color col = isWalkable(m->grid[y][x]) ? (Color){230,230,230,255}
                                                  : (Color){50,50,70,255};
            DrawRectangleRec(r, col);
        }
    }
    // grid overlay
    for(int y=0;y<=m->h;++y) DrawLine(0,y*TILE,m->w*TILE,y*TILE,(Color){30,30,40,80});
    for(int x=0;x<=m->w;++x) DrawLine(x*TILE,0,x*TILE,m->h*TILE,(Color){30,30,40,80});
}

void DrawExit(const Maze* m){
    Rectangle r={ m->exit.x*TILE+8, m->exit.y*TILE+8, TILE-16, TILE-16 };
    DrawRectangleRec(r, (Color){255,215,0,255});
    DrawRectangleLinesEx(r, 2, (Color){160,120,0,255});
}