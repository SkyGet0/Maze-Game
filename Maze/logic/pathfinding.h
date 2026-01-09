#pragma once
#include <stdbool.h>
#include "../settings/types.h"

bool NextStepBFS(const Maze* m, P2 from, P2 to, P2* outDir);
const char* DirToText(P2 d);