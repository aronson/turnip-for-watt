#pragma once
#import "seven/input.h"

#define FLOOR_Y (160-16)
const int WALK_SPEED = 2;
const int JUMP_SPEED = 20;
const int GRAVITY = 1;
const int MAX_X_POS = 240 - 16;
const int MAX_Y_VEL = 5;
const int ATTRIBUTE_RIGHT_MASK = 0x4000;
const int ATTRIBUTE_LEFT_MASK = 0x5000;

typedef struct PhysicsData {
    int facingRight{};
    int posX{};
    int posY{};
    int velX{};
    int velY{};
    bool isMidAir = false;
    int framesInAir{};
    void updatePhysics();

private:
    void applyInputVelocities();
    void moveSpriteHorizontally();
    void moveSpriteVertically();
} PhysicsData;

