#pragma once

#include "seven/types.h"
#include "aeabi.h"

#include <cstdint>
#include <stdexcept>
#include <bitset>

#define MEM_VRAM 0x06000000

#define MEM_PALETTE_BG  ((u16 *)(0x05000000))
#define MEM_PALETTE_OAM ((u16 *)(0x05000200))
#define BG_CBB_SHIFT    2
#define BG_CBB(n)       ((n)<<BG_CBB_SHIFT)
#define BG_SBB_SHIFT    8
#define BG_SBB(n)       ((n)<<BG_SBB_SHIFT)


typedef struct ScreenBlock {
    u16 tiles[1024];
} ScreenBlock;

typedef struct Tile {
    u16 tiles[16];
} Tile;

typedef struct Charblock{
    Tile tiles[512];
} Charblock;

typedef struct VRAMAllocation{
    Charblock charblocks[4];
    Charblock spriteblocks[2];
} VRAMAllocator;

extern VRAMAllocation vram;
extern std::bitset<32> allocations;

typedef struct BackgroundAllocation {
    Tile* tiles;
    size_t tileLength;
    ScreenBlock* screenBlock;
    u32 charBlockIndex;
    u32 screenBlockIndex;
} BackgroundAllocation;

BackgroundAllocation allocate_tiles(u32 charblock_index, unsigned int num_tiles);
void dirty_tiles(BackgroundAllocation allocation);
