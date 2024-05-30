#include "vram_sections.hpp"

__attribute((section(".vram"))) VRAMAllocation vram;
std::bitset<32> allocations{};

BackgroundAllocation allocate_tiles(u32 charblock_index, unsigned int num_tiles) {
    auto *cb = (Charblock *)vram.charblocks;
    auto *sb = (ScreenBlock *)vram.charblocks;
    for (u32 i = 0; i < sizeof(ScreenBlock); i += num_tiles) {
        if (allocations[i] == 0) { // Check if the space is free
            size_t tileLength = num_tiles * sizeof(Tile);
            u32 screenEntriesUsedByTiles = (((sizeof(ScreenBlock) * 32 + tileLength - 1) / tileLength) + (sizeof(ScreenBlock) - 1)) / sizeof(ScreenBlock);
            u32 j;
            for (j = charblock_index * 8; j < screenEntriesUsedByTiles; j++)
                allocations.flip(j);
            return {&cb->tiles[i], tileLength, &sb[j], charblock_index, j};
        }
    }
    return {nullptr, 0, nullptr, 0, 0}; // No free space found
}

void dirty_tiles(BackgroundAllocation allocation) {
    u32 screenEntriesUsedByTiles = (((sizeof(ScreenBlock) * 32 + allocation.tileLength - 1) / allocation.tileLength) +
            (sizeof(ScreenBlock) - 1)) / sizeof(ScreenBlock);
    for (u32 j = allocation.charBlockIndex * 8; j < screenEntriesUsedByTiles; j++)
        allocations.flip(j);
}
