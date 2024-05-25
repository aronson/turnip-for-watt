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
        framesInAir = 0;
    }
}

void PhysicsData::moveSpriteHorizontally() {
    posX += velX;
    posX = std::min(MAX_X_POS, posX);
    posX = std::max(0, posX);
}

void PhysicsData::moveSpriteVertically() {
    if (isMidAir) {
        velY = JUMP_SPEED + (GRAVITY * framesInAir);
        velY = std::min(MAX_Y_VEL, velY);
        framesInAir++;
    }

    posY += velY;
    posY = std::min(posY, FLOOR_Y);
}

void PhysicsData::updatePhysics() {
    applyInputVelocities();
    moveSpriteHorizontally();
    moveSpriteVertically();
}