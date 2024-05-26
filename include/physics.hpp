#pragma once
#import "seven/input.h"
#define LIBSEVEN_COMPATIBLE
#import "seven/types.h"

#define FLOOR_Y (160-16)
const u8 WALK_SPEED = 2;
const u8 JUMP_SPEED = 5;
const u8 GRAVITY = 1;
const u8 MAX_X_POS = 240 - 16;
const u8 MAX_Y_VEL = 5;

typedef struct PhysicsData {
    int facingRight{};
    u8 posX{};
    u8 posY{};
    s16 velX{};
    s16 velY{};
    bool isMidAir = false;
    int airTime{};
    void updatePhysics();

private:
    void applyInputVelocities();
    void moveSpriteHorizontally();
    void moveSpriteVertically();
} PhysicsData;

