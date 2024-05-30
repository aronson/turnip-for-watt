#include "actor.hpp"
#include "jimmyShared.h"
#include "dorothy.h"
#include "dorothyShared.h"
#include "vram_sections.hpp"

void Actor::init(Object *oam, Object *buffer) {
    immediate = oam;
    shadow = buffer;

    // Register default OAM data
    shadow->attr0 = ATTR0_4BPP;
    shadow->attr1 = ATTR1_FACING_RIGHT;
    shadow->attr2 = 0;

    physics.facingRight = 1;
    physics.posY = FLOOR_Y;
}

void Actor::draw() {
    if (!initialized) {
        // Set up main sprite graphics data
        __agbabi_memcpy2((void *)&vram.spriteblocks, jimmyTiles, jimmyTilesLen);
        __agbabi_memcpy2(MEM_PALETTE_OAM, jimmySharedPal, jimmySharedPalLen);
        initialized = true;
    }

    // Always copy actor in the draw step to OAM
    __agbabi_memcpy2(immediate, shadow, sizeof(Object));
}

void Actor::update() {
    animate();
}

void Actor::animate() {
    shadow->attr0 = ATTR0_4BPP + physics.posY;
    shadow->attr1 = physics.posX +
                    (physics.facingRight ? ATTR1_FACING_RIGHT : ATTR1_FACING_LEFT);

    if (physics.isMidAir) {
        firstAnimCycleFrame = MIDAIR_START_FRAME;
        animFrame = physics.velY > 0 ? 1 : 0;
    } else {
        if (physics.velX != 0) {
            firstAnimCycleFrame = WALKING_START_FRAME;
            if (!(animTimer++ % 8)) {
                ++animFrame;
                animFrame %= 3;
            }
        } else {
            firstAnimCycleFrame = 0;
            if (!(animTimer++ % 8)) {
                ++animFrame;
                animFrame %= 4;
            }
        }
    }

    shadow->attr2 = firstAnimCycleFrame + (animFrame * 4);
}

void Actor::control() {
    physics.updatePhysics();
}
