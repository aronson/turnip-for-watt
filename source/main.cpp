#include "def.hpp"
#include "seven/irq.h"
#include "seven/bios.h"
#include "seven/video.h"
#include "seven/memory.h"
#include "seven/attributes.h"
#include "seven/waitstate.h"
#include "seven/types.h"
#include "gbfs.h"
#include "maxmod.h"

#include "scene.hpp"
#include "soundbank.h"

using namespace hsm;

EWRAM_DATA StateMachine stateMachine;
EWRAM_DATA InputState keys = inputNew();
std::shared_ptr<Scene> scene = nullptr;
EWRAM_DATA LogInterface loggerType = LOGIF_CUSTOM;
EWRAM_DATA std::shared_ptr<u8> soundbank = nullptr;

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
    mmVBlank();
    if (canDraw) {
        canDraw = false;
        if (scene) scene->draw();
    }
    mmFrame();
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

u8 * findSoundbank(){
    return (u8 *) gbfs_get_obj(
            find_first_gbfs_file(reinterpret_cast<const void *>(MEM_ROM)),
                "soundbank.bin", nullptr);
}

mm_word mmEventHandler(mm_word msg, mm_word param){
    return 0;
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

    u8* result = findSoundbank();
    if (result)
        soundbank = std::shared_ptr<u8>(result);

    mmInitDefault((mm_addr)soundbank.get(), 12);
    mmSetEventHandler((mm_callback)mmEventHandler);

    mmStart(0, MM_PLAY_LOOP);
    mmSetModuleVolume(1024);
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
