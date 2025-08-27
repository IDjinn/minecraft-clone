//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
#include <unordered_map>
#include <vector>

#include "chunks/Chunk.h"
#include "glm/vec3.hpp"
#include "../players/Player.h"

struct Player;
struct Chunk;

struct WorldCoord {
    int32_t x, y, z;
};

struct World : public std::enable_shared_from_this<World>{
    uint8_t id;
    glm::vec3 spawn_point;
    std::vector<std::shared_ptr<Player>> players;
    std::unordered_map<int32_t, std::unique_ptr<Chunk>> chunks{};
    std::unordered_map<int32_t, std::unique_ptr<std::vector<float>>> chunk_visible_vertices{};

    World(uint8_t id, const glm::vec3 &spawn_point);

    void add_player(const std::shared_ptr<Player> &player);

    static uint32_t generate_entity_id();

    std::unique_ptr<std::vector<float>> generate_visible_vertices();

    void check_chunk_lifetimes(const glm::vec3 center_position);
    void load_chunk(int32_t chunk_id);
    void unload_chunk(const int32_t id);
    [[nodiscard]] bool isChunkLoaded(int32_t chunk_id);
    [[nodiscard]] Chunk &get_chunk(int32_t chunk_id);
    [[nodiscard]] Chunk &get_chunk(WorldCoord coords);

    static constexpr int32_t chunk_id_from_world_coords(const WorldCoord coord);
    static constexpr WorldCoord chunk_id_to_world_coordinates(const int32_t id);
    static constexpr bool isOutOfBounds(const int x, const int y, const int z);
};



#endif //MINECRAFT_WORLD_H
