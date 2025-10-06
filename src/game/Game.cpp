//
// Created by Luke on 26/08/2025.
//

#include "Game.h"
#include "../utils/PerformanceTimer.h"

Game::Game() : player(std::make_shared<Player>(World::generate_entity_id(), "IDjinn", WORLD_SPAWN_COORDS)),
               world(std::make_shared<World>(OVERWORLD, WORLD_SPAWN_COORDS)),
               render(std::make_shared<Render>(this->world)) {
    PRINT_DEBUG("loading game..\nDEBUG: " << MINECRAFT_DEBUG << "\nWORLD_DEBUG: " << WORLD_DEBUG_FLAG);
    player->current_world = world;
    world->add_player(player);
    world->check_chunk_lifetimes(player->position);
    running = true;
}

void Game::run() const {
    PRINT_DEBUG("game is loaded. init rendering!"<<std::flush);

    while (running && render->is_running()) {
        this->runPhysics();
        this->render->runRender();
    }

    // TODO: saves e etc
}

void Game::runPhysics() const {
    if (this->world->pending_chunks.empty())return;

    for (auto it = this->world->pending_chunks.begin();
         it != this->world->pending_chunks.end();) {
        if (it->second.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::ready) {
            int32_t chunk_id = it->first;

            auto chunk = it->second.get();
            this->world->chunks[chunk_id] = std::move(chunk);
            it = this->world->pending_chunks.erase(it);
        } else {
            ++it;
        }
    }
}
