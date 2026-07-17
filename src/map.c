//
// Created by green on 2026-07-14.
//

#include "map.h"
#include <stdlib.h>
#include "extern/stb_ds.h"

Chunk generate_chunk(const Checker coord) {
    Chunk chunk = {
        .coord = coord,
        .layers = {0}
    };

    // create inlets
    const size_t msize = sizeof(Tile) * CHUNK_SIZE * (CHUNK_SIZE * 2);
    chunk.layers[0] = malloc(msize);
    memset(chunk.layers[0], 0, msize);

    for (int i = 0; i < 6; ++i) {
        const int c = GetRandomValue(0, CHUNK_SIZE);
        const int r = GetRandomValue(0, CHUNK_SIZE) + c % 2;
        chunk.layers[0][CHECKER2INDEX(c, r)] = (Tile) {
            .type = TF_CAN_INTERACT | TF_CAN_BUILD
        };
    }

    return chunk;
}

void delete_chunk(const Chunk *chunk) {
    for (int i = 0; i < CHUNK_LAYER_COUNT; ++i) {
        if (chunk->layers[i] != nullptr) { free(chunk->layers[i]); }
    }
}

Axial GetMapNeighbor(const Axial coord, const AxialDirection neighborDirection) {
    return AxialAdd(coord, hexDirections[neighborDirection]);
}

Tile * get_chunk_tile(const Chunk *chunk, const Checker coord) {
    const unsigned int index = CHECKER2INDEX(coord.col, coord.row);
    if (index >= CHUNK_SIZE * CHUNK_SIZE) { return nullptr; }
    return &chunk->layers[0][CHECKER2INDEX(coord.col, coord.row)];
}

bool check_chunk_collision(const Chunk *chunk, const Axial coord) {
    // if (OUT_OF_BOUND(map, coord)) { return true; }
    return false;
}

bool is_tile_free(Chunk *chunk, Axial coord) {
    return true;
}

Axial PathNextMapCoordinate(const Chunk *chunk, const Axial from, const Axial to) {
    return from;
}