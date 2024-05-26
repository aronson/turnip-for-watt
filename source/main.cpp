#include "def.hpp"
#include "seven/irq.h"
#include "seven/bios.h"
#include "seven/video.h"
#include "seven/attributes.h"
#include "seven/waitstate.h"

#include "scene.hpp"

using namespace hsm;

EWRAM_DATA StateMachine stateMachine;
EWRAM_DATA InputState keys = inputNew();
std::shared_ptr<Scene> scene = nullptr;
EWRAM_DATA LogInterface loggerType = LOGIF_CUSTOM;

bool canDraw = true;

struct GameStates {
    struct RootState : State {
        void OnEnter() override {
            changeScene(new GameScene());
        }
        void Update() override {
            if (scene)  {
                scene->control();
                scene->update();
            }
        }
        Transition GetTransition() override {
            return NoTransition();
        }
    };
};

ARM_CODE IWRAM_CODE void onVBlank(u16 _) {
    // mmVBlank();
    if (canDraw) {
        canDraw = false;
        if (scene) scene->draw();
    }
    // mmFrame();
}

void initializeStateMachine() {
    stateMachine.SetDebugTraceLevel(hsm::TraceLevel::Basic);
    stateMachine.Initialize<GameStates::RootState>(nullptr);
    irqHandlerSet(IRQ_VBLANK, onVBlank);
}

void alignInitialVideoTiming() {
    while (REG_VCOUNT != 160);
    while (REG_VCOUNT != 161);
}

void platformInit() {
    //testSaveMedia();

    REG_DISPSTAT |= (1 << 3) | (1 << 4);

    // Set display mode
    REG_DISPCNT = DISPLAY_BG0_ENABLE | DISPLAY_OBJ_LAYOUT_1D | DISPLAY_MODE_REGULAR | DISPLAY_OBJ_ENABLE;

    // Conservative overclock
    REG_WAITCNT = 0x4317;

    // Set up interrupts
    alignInitialVideoTiming();
    irqInitDefault();
    u16 interrupts = IRQ_VBLANK;
    irqEnable(interrupts);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
IWRAM_CODE int main() {

    platformInit();

    initializeStateMachine();
    do {
        stateMachine.ProcessStateTransitions();
        stateMachine.UpdateStates();
        canDraw = true;
        biosVBlankIntrWait();
    } while (true);
}
#pragma clang diagnostic pop
