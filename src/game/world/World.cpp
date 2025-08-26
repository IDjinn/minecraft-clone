//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include "../../utils/Assert.h"


Chunk &World::getChunk(const WorldCoord coords) {
    return getChunk(chunk_id_from_world_coords(coords));
}

Chunk &World::getChunk(ChunkId chunk_id) {
    auto it = chunks.find(chunk_id);
    ASSERT(it != chunks.end(), "Chunk was not found/loaded");
    return *it->second;
}

bool World::isChunkLoaded(ChunkId chunk_id) {
    return chunks.find(chunk_id) != chunks.end();
}

bool World::loadChunk(ChunkId chunk_id) {
    if (isChunkLoaded(chunk_id)) return false;

    chunks[chunk_id] = std::make_unique<Chunk>(chunk_id);
    return true;
}