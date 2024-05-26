#include "def.hpp"
#include "physics.hpp"

void PhysicsData::applyInputVelocities() {
    // Grab input
    i8 axisX = inputAxisX(keys);

    // Check if running
    if(inputKeysDown(KEY_B, keys)) {
        axisX *= 2;
    }

    // Update direction and velocity based on user input
    if (axisX != 0) {
        facingRight = axisX > 0 ? 1 : 0;
    }
    velX = WALK_SPEED * axisX;

    // Test if actor is in the air
    isMidAir = posY != FLOOR_Y;

    if (inputKeysPressed(KEY_A, keys) && !isMidAir) {
        velY = -JUMP_SPEED;
        airTime = 0;
    }
}

void PhysicsData::moveSpriteHorizontally() {
    posX += velX;
    posX = std::min(MAX_X_POS, posX);
    posX = std::max((u8)0, posX);
}

void PhysicsData::moveSpriteVertically() {
    velY += GRAVITY;
    velY = std::min((i16)MAX_Y_VEL, velY);

    posY += velY;

    if (posY >= FLOOR_Y) {
        posY = FLOOR_Y;
        isMidAir = false;
        velY = 0;
    } else {
        isMidAir = true;
        airTime++;
    }
}

void PhysicsData::updatePhysics() {
    applyInputVelocities();
    moveSpriteHorizontally();
    moveSpriteVertically();
}
