//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
#include <unordered_map>
#include <vector>
#include <expected>
#include <future>

#include "glm/vec3.hpp"
#include "../players/Player.h"
#include "chunks/ChunkError.h"
#include "generation/WorldGeneration.h"
struct WorldGeneration;
struct Player;
struct Chunk;
struct WorldCoord;


struct World : public std::enable_shared_from_this<World>{
    uint8_t id;
    glm::vec3 spawn_point;
    std::vector<std::shared_ptr<Player>> players;
    std::unordered_map<int32_t, std::unique_ptr<Chunk>> chunks{};
    std::unordered_map<int32_t, std::future<std::unique_ptr<Chunk> > > pending_chunks{};
    std::unordered_map<int32_t, std::vector<float> > chunk_visible_vertices{};
    std::unique_ptr<WorldGeneration> world_generation;

    World(uint8_t id, const glm::vec3 &spawn_point);

    ~World();

    void add_player(const std::shared_ptr<Player> &player);

    static uint32_t generate_entity_id();

    std::unique_ptr<std::vector<float>> generate_visible_vertices();

    std::vector<int32_t> get_visible_chunk_ids(glm::vec3 center_position);

    void unload_chunks(const std::vector<int32_t> &chunk_ids);

    void check_chunk_lifetimes(glm::vec3 center_position);

    void unload_chunk(int32_t id);

    [[nodiscard]] bool is_chunk_loaded(int32_t chunk_id);

    [[nodiscard]] std::expected<std::reference_wrapper<Chunk>, ChunkError> get_chunk(WorldCoord coords);

    std::expected<std::reference_wrapper<Chunk>, ChunkError> get_chunk(int32_t chunk_id);
};



#endif //MINECRAFT_WORLD_H
