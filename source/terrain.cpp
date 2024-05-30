#include "terrain.hpp"

TerrainElement terrain[TERRAIN_COUNT];

void loadTerrain() {
    __agbabi_memcpy2(terrain, nullptr, sizeof(terrain));
}
