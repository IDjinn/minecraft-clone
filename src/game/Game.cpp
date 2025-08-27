//
// Created by Luke on 26/08/2025.
//

#include "Game.h"

Game::Game() : player(std::make_shared<Player>(World::generate_entity_id(), "IDjinn", WORLD_SPAWN_COORDS)),
               world(std::make_shared<World>(OVERWORLD, WORLD_SPAWN_COORDS)),
               render(std::make_shared<Render>(this->world)) {
    PRINT_DEBUG("loading game..\nDEBUG: " << MINECRAFT_DEBUG << "\nWORLD_DEBUG: " << WORLD_DEBUG_FLAG);
    world->check_chunk_lifetimes(WORLD_SPAWN_COORDS);
    player->current_world = world;
    world->add_player(player);
}

void Game::run() const {
    PRINT_DEBUG("game is loaded. init rendering!");
    WHEN_DEBUG(std::cout<<std::flush);
    while (render->is_running()) {
        render->render();
    }
}
