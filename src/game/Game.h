//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_GAME_H
#define MINECRAFT_GAME_H
#include "../render/Render.h"


struct Game {
    World world;
    Render render;
    std::shared_ptr<Player> player = std::make_shared<
        Player>(World::generate_entity_id(), "IDjinn", WORLD_SPAWN_COORDS);

    Game() : world(OVERWORLD, WORLD_SPAWN_COORDS), render(this->world) {
        world.add_player(player);
    }

    void run() {
        while (render.is_running()) {
            render.render();
        }
    }
};


#endif //MINECRAFT_GAME_H
