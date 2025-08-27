//
// Created by Luke on 26/08/2025.
//

#include "Game.h"

Game::Game() : player(std::make_shared<Player>(World::generate_entity_id(), "IDjinn", WORLD_SPAWN_COORDS)),
               world(std::make_shared<World>(OVERWORLD, WORLD_SPAWN_COORDS)),
               render(std::make_shared<Render>(this->world)) {
    world->check_chunk_lifetimes(WORLD_SPAWN_COORDS);
    player->current_world = world;
    world->add_player(player);
}

void Game::run() const {
    while (render->is_running()) {
        render->render();
    }
}
