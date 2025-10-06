//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_GAME_H
#define MINECRAFT_GAME_H
#include <atomic>

#include "../render/Render.h"


struct Game {
    std::shared_ptr<Player> player;
    std::shared_ptr<World> world;
    std::shared_ptr<Render> render;

    std::atomic<bool> running;

    Game();

    void run() const;

private:
    void runPhysics() const;
};


#endif //MINECRAFT_GAME_H
