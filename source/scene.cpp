#include "seven/video/memory.h"
#include "seven/attributes.h"
#include "def.hpp"
#include "scene.hpp"
#include "jimmy.h"

void GameScene::init() {
    jimmy.init(&MEM_SPRITE[0], jimmyShadow);
}

ARM_CODE IWRAM_CODE void GameScene::draw() {
    jimmy.draw();
}

void GameScene::update() {
    jimmy.update();
}
bool GameScene::control() {
    keys = inputPoll(keys);
    jimmy.control();
    return true;
}

void GameScene::deinit() {
    // Dellocate sprite
}