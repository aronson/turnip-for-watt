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
        airTime = 0;
    }

    if (inputKeysDown(KEY_A, keys) && airTime < 10) {
        velY = -JUMP_SPEED;
    }
}

void PhysicsData::moveSpriteHorizontally() {
    if (posX + velX < 0) {
        posX = 0;
    } else if (posX + velX > MAX_X_POS) {
        posX = MAX_X_POS;
    } else {
        posX += velX;
    }
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
