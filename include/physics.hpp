#pragma once
#include "seven/input.h"
#include "seven/types.h"
#include "realnumb/fixed.hpp"

using namespace realnumb;

#define FLOOR_Y (160-16)
const fixed32 WALK_SPEED = 2;
const fixed32 JUMP_SPEED = 5;
const fixed32 GRAVITY = 0.5;
const fixed32 MAX_X_POS = 240 - 16;
const fixed32 MAX_Y_VEL = 4.2;

typedef struct PhysicsData {
    int facingRight{};
    u8 posX{};
    u8 posY{};
    fixed32 velX{};
    fixed32 velY{};
    bool isMidAir = false;
    int airTime{};
    void updatePhysics();

private:
    void applyInputVelocities();
    void moveSpriteHorizontally();
    void moveSpriteVertically();
} PhysicsData;

