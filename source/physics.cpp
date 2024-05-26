#import "def.hpp"
#import "physics.hpp"

void PhysicsData::applyInputVelocities() {
    // Grab input
    const int axisX = inputAxisX(keys);

    // Update direction and velocity based on user input
    if (axisX == -1) {
        facingRight = 0;
        velX = -WALK_SPEED;
    } else if (axisX == 1) {
        facingRight = 1;
        velX = WALK_SPEED;
    } else {
        velX = 0;
    }

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
    velY = std::min((s16)MAX_Y_VEL, velY);

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
