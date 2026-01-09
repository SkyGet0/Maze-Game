/*
    Собираем игру из сборища всех .c файлов.
    Да, это костыль, но проще собрать игру так, чем
    редачить конфигурацию всего билда.
*/

#include "logic/game.c"
#include "logic/maze.c"
#include "ui/ui.c"
#include "logic/pathfinding.c"
#include "main.c"