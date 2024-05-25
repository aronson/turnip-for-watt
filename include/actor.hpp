#pragma once

#include "seven/types.h"
#include "seven/memory.h"
#include "seven/video.h"
#include "seven/video/types.h"
#include "agbabi.h"
#include "def.hpp"
#include "physics.hpp"
#include "jimmy.h"

typedef u16 Tile[32];
typedef Tile TileBlock[256];
#define MEM_TILE ((TileBlock*)MEM_VRAM)
#define MEM_PALBLOCK ((Palette *)MEM_PALETTE)
#define MEM_SPRITE  ((Object *)MEM_OAM)

#define WALKING_START_FRAME 32
#define MIDAIR_START_FRAME 56

class Jimmy {
private:
    unsigned int firstAnimCycleFrame = 0;
    unsigned int animFrame = 0;
    unsigned int animTimer = 0;
    bool initialized = false;
public:
    Object *shadow{};
    Object *immediate{};
    PhysicsData physics{};

    void init(Object *oam, Object *buffer) {
        immediate = oam;
        shadow = buffer;

        // Register default OAM data
        shadow->attr0 = 0x2000;
        shadow->attr1 = 0x4000;
        shadow->attr2 = 0;

        physics.facingRight = 1;
        physics.posY = FLOOR_Y;
    }

    void draw() {
        if (!initialized) {
            // Set up main sprite graphics data
            __agbabi_memcpy2(MEM_TILE[4][0], jimmyTiles, jimmyTilesLen);
            __agbabi_memcpy2(MEM_PALBLOCK[1], jimmyPal, jimmyPalLen);
            initialized = true;
        }

        // Always copy actor in the draw step to OAM
        __agbabi_memcpy2(immediate, shadow, sizeof(Object));
    }

    void update() {
        animate();
    }

    void animate() {
        unsigned int originalFrame = firstAnimCycleFrame;
        //update velocity for gravity
        if (physics.isMidAir) {
            firstAnimCycleFrame = MIDAIR_START_FRAME;
            animFrame = physics.velY > 0 ? 1 : 0;
        } else {
            if (physics.velX != 0) {
                firstAnimCycleFrame = WALKING_START_FRAME;
                ++animFrame;
                animFrame %= 3;
            } else {
                firstAnimCycleFrame = 0;
                ++animFrame;
                animFrame %= 4;
            }
        }

        shadow->attr0 = 0x2000 + physics.posY;
        shadow->attr1 = physics.posX +
                        (physics.facingRight ? ATTRIBUTE_RIGHT_MASK : ATTRIBUTE_LEFT_MASK);
        if (!(animTimer++ % 8) || (originalFrame != firstAnimCycleFrame))
            shadow->attr2 = firstAnimCycleFrame + (animFrame * 8);
    }

    void control() {
        physics.updatePhysics();
    }
};
