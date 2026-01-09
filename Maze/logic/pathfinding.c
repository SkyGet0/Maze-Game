#include "pathfinding.h"
#include <string.h>

bool NextStepBFS(const Maze* m, P2 from, P2 to, P2* outDir){
    static int qx[MAP_W*MAP_H], qy[MAP_W*MAP_H];
    static short px[MAP_H][MAP_W], py[MAP_H][MAP_W];
    static unsigned char vis[MAP_H][MAP_W];
    memset(vis,0,sizeof(vis));
    for(int y=0;y<MAP_H;++y) for(int x=0;x<MAP_W;++x){ px[y][x]=-1; py[y][x]=-1; }

    int h=0,t=0;
    qx[t]=from.x; qy[t]=from.y; t++;
    vis[from.y][from.x]=1;

    const int dirs[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
    while(h<t){
        int cx=qx[h], cy=qy[h]; h++;
        if (cx==to.x && cy==to.y) break;
        for(int i=0;i<4;++i){
            int nx=cx+dirs[i][0], ny=cy+dirs[i][1];
            if (nx<0 || ny<0 || nx>=MAP_W || ny>=MAP_H) continue;
            if (vis[ny][nx]) continue;
            if (m->grid[ny][nx] != '.') continue;
            vis[ny][nx]=1; px[ny][nx]=cx; py[ny][nx]=cy;
            qx[t]=nx; qy[t]=ny; t++;
        }
    }
    if(!vis[to.y][to.x]) return false;

    int cx=to.x, cy=to.y;
    int pcx=-1,pcy=-1;
    while(!(cx==from.x && cy==from.y)){
        pcx=px[cy][cx]; pcy=py[cy][cx];
        if (pcx==from.x && pcy==from.y) break;
        cx=pcx; cy=pcy;
    }
    if(pcx==-1) return false;
    outDir->x = cx - from.x;
    outDir->y = cy - from.y;
    return true;
}

const char* DirToText(P2 d){
    if      (d.x== 1 && d.y== 0) return "Right";
    else if (d.x==-1 && d.y== 0) return "Left";
    else if (d.x== 0 && d.y== 1) return "Down";
    else if (d.x== 0 && d.y==-1) return "Up";
    return "None";
}