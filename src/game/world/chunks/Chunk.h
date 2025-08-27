//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include <array>
#include <memory>

#include "../World.h"
#include "../blocks/Block.h"
#include "../WorldConstants.h"

struct World;

enum class ChunkState {
    UNKNOWN = 0,

    LOADED = 1,
    INITIALIZED = 2,
};

struct Chunk {
    int32_t id{};
    std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> blocks{};
    ChunkState state = ChunkState::UNKNOWN;

    std::weak_ptr<World> world_ptr;

    Chunk(const int32_t id, const std::weak_ptr<World> &world_ptr) : id(id), world_ptr(world_ptr) {
    }

    ~Chunk();

    Chunk(const Chunk &) = delete;

    Chunk &operator=(const Chunk &) = delete;

    std::unique_ptr<std::vector<float> > generate_visible_vertices();

    [[nodiscard]] const Block *getBlock(const int x, const int y, const int z) const;

    [[nodiscard]] const Block *getBlock(const uint8_t index) const;

    [[nodiscard]] ChunkState getState() const;

    void initialize_blocks();

    void setState(const ChunkState newState);

    void set_index(uint8_t chunk_index);

    static constexpr uint32_t block_index(const uint32_t x, const uint32_t y, const uint32_t z) {
        return x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
    }
};


#endif //MINECRAFT_CHUNK_H
