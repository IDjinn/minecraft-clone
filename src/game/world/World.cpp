//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include "../../utils/Assert.h"


Chunk &World::getChunk(const int x, const int y, const int z) {
    const auto chunk_id = chunk_index(x, y, z);
    return getChunk(chunk_id);
}

Chunk &World::getChunk(uint8_t chunk_index) {
    auto it = chunks.find(chunk_index);
    ASSERT(it != chunks.end(), "Chunk was not found/loaded");
    return *it->second;
}

bool World::isChunkLoaded(uint8_t chunk_index) {
    return chunks.find(chunk_index) != chunks.end();
}

bool World::loadChunk(uint8_t chunk_index) {
    if (isChunkLoaded(chunk_index)) return false;

    chunks[chunk_index] = std::make_unique<Chunk>(chunk_index);
    return true;
}