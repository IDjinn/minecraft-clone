//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_GAME_H
#define MINECRAFT_GAME_H
#include "../render/Render.h"


struct Game {
    std::shared_ptr<Player> player;
    std::shared_ptr<World> world;
    std::shared_ptr<Render> render;

    Game();

    void run() const;
};


#endif //MINECRAFT_GAME_H
