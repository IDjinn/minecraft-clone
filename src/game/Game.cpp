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

    std::unordered_set<int32_t> pending_to_remove{};
    pending_to_remove.reserve(this->world->pending_chunks.size());
    for (auto &[chunk_id, future]: this->world->pending_chunks) {
        if (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) continue;

        auto chunk = future.get();
        this->world->chunks[chunk_id] = std::move(chunk);
        pending_to_remove.insert(chunk_id);
    }

    for (auto chunk_id: pending_to_remove) {
        this->world->pending_chunks.erase(chunk_id);
    }
}
