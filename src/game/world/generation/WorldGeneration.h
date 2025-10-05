//
// Created by Luke on 27/08/2025.
//

#ifndef MINECRAFT_WORLDGENERATION_H
#define MINECRAFT_WORLDGENERATION_H
#include <cstdint>
#include <memory>
#include <unordered_set>

#include "glm/vec3.hpp"
#include "FastNoise/FastNoise.h"
struct Chunk;
struct World;

struct WorldGeneration {
    std::vector<float> heightMap;
    FastNoise::SmartNode<FastNoise::Simplex> fnGenerator;
    long seed;

    explicit WorldGeneration(long seed);

    void load_chunk(const std::unique_ptr<Chunk> &chunk);

    std::unordered_map<int32_t, std::unique_ptr<Chunk> > generate_chunks_around(
     const std::shared_ptr<World> &world,
     glm::vec3 position
 );
};


#endif //MINECRAFT_WORLDGENERATION_H
