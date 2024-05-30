#include "seven/video/memory.h"
#include "seven/video/bg_scroll.h"
#include "seven/attributes.h"
#include "def.hpp"
#include "scene.hpp"
#include "CandyLand.h"
#include "CandyLandMap.h"
#include "vram_sections.hpp"
#include "graphics.hpp"

void GameScene::init() {
    oamInitAll();
    jimmy.init(&((Object *)MEM_OAM)[0], jimmyShadow);
}


ARM_CODE IWRAM_CODE void GameScene::draw() {
    if (!backdropInitialized) {
        allocation = allocate_tiles(0, CandyLandTilesLen / sizeof(Tile));

        REG_BG0CNT = BackgroundControl::BG_TILE_4BPP | BackgroundControl::BG_SIZE_256X256;
        REG_BG0CNT |= BG_SBB(allocation.screenBlockIndex) | BG_CBB(allocation.charBlockIndex);

        __agbabi_memcpy2((void *) allocation.tiles, CandyLandTiles, CandyLandTilesLen);
        __agbabi_memcpy2((void *) allocation.screenBlock, CandyLandMap, sizeof(CandyLandMap));

        __agbabi_memcpy2(&MEM_PALETTE_BG[0], CandyLandPal, CandyLandPalLen);

        REG_BG0HOFS = 0;
        REG_BG0VOFS = 92;

        backdropInitialized = true;
    }

    jimmy.draw();
}

void GameScene::update() {
    jimmy.update();
}
bool GameScene::control() {
    keys = inputPoll(keys);
    jimmy.control();
    return true;
}

void GameScene::deinit() {
    dirty_tiles(allocation);
}