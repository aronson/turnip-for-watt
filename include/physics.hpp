#pragma once
#include "seven/input.h"
#include "seven/types.h"

#define FLOOR_Y (160-16)
const i8 WALK_SPEED = 2;
const u8 JUMP_SPEED = 10;
const u8 GRAVITY = 1;
const u8 MAX_X_POS = 240 - 16;
const u8 MAX_Y_VEL = 5;

typedef struct PhysicsData {
    int facingRight{};
    u8 posX{};
    u8 posY{};
    i16 velX{};
    i16 velY{};
    bool isMidAir = false;
    int airTime{};
    void updatePhysics();

private:
    void applyInputVelocities();
    void moveSpriteHorizontally();
    void moveSpriteVertically();
} PhysicsData;

