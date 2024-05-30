#pragma once

#include "seven/types.h"
#include "seven/memory.h"
#include "seven/video.h"
#include "seven/attributes.h"
#include "seven/video/types.h"
#include "agbabi.h"
#include "def.hpp"
#include "physics.hpp"
#include "jimmy.h"

#define WALKING_START_FRAME 16
#define MIDAIR_START_FRAME 28

const int ATTR1_SQUARE = 0x4000;
const int ATTR1_FLIP_HORIZ = 0x1000;
const int ATTR1_FACING_LEFT = ATTR1_SQUARE + ATTR1_FLIP_HORIZ;
const int ATTR1_FACING_RIGHT = ATTR1_SQUARE;
const int ATTR0_4BPP = 0x0000;

class Actor {
private:
    unsigned int firstAnimCycleFrame = 0;
    unsigned int animFrame = 0;
    unsigned int animTimer = 0;
    bool initialized = false;
public:
    Object *shadow{};
    Object *immediate{};
    PhysicsData physics{};

    void init(Object *oam, Object *buffer);
    ARM_CODE IWRAM_CODE void draw();
    void update();
    void animate();
    void control();
};
