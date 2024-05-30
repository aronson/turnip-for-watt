#pragma once

#include <memory>
#include "agbabi.h"

#define TERRAIN_COUNT (32*32)

enum TerrainType {
    Clear,
    Square
};

typedef struct TerrainElement {
    TerrainType type;
} TerrainElement;

extern TerrainElement terrain[TERRAIN_COUNT];

void loadTerrain();
