//
// Created by Luke on 27/08/2025.
//

#ifndef MINECRAFT_WORLDGENERATION_H
#define MINECRAFT_WORLDGENERATION_H
#include <cstdint>
#include <future>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "glm/vec3.hpp"
#include "FastNoise/FastNoise.h"
struct Chunk;
struct World;

struct WorldGeneration {
    std::vector<float> heightMap;
    FastNoise::SmartNode<FastNoise::Simplex> fnGenerator;
    std::mutex generator_mutex;
    long seed;

    explicit WorldGeneration(long seed);

    void load_chunk(const std::unique_ptr<Chunk> &chunk);

    static std::unordered_set<int32_t> chunk_ids_around_position(glm::vec3 position, glm::vec3 render_distance);

    std::unordered_map<int32_t, std::future<std::unique_ptr<Chunk> > > load_chunks_async(
        const std::shared_ptr<World> &world, const std::vector<int32_t> &chunk_ids
    );
};


#endif //MINECRAFT_WORLDGENERATION_H