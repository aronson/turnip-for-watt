#pragma once
#include <memory>
#include "physics.hpp"
#include "actor.hpp"

class Scene{
public:
    virtual void draw() = 0;
    virtual bool control() = 0;
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void deinit() = 0;
    virtual std::shared_ptr<Scene> previousScene() = 0;

    Scene()= default;

    virtual ~Scene()= default;
};

extern std::shared_ptr<Scene> scene;

inline void changeScene(Scene * newScene) {
    scene = std::shared_ptr<Scene>(newScene);
    scene->init();
}

class GameScene final : public Scene{
    void draw() final;
    bool control() final;
    void init() final;
    void update() final;
    void deinit() final;

    Object spriteShadowBuffer[128]{};
    Object *jimmyShadow = &spriteShadowBuffer[0];

    bool initialized = false;
    Jimmy jimmy;

    std::shared_ptr<Scene> previousScene() final {return nullptr;}

    ~GameScene() final{
        deinit();
    };
};