//
// Created by green on 2026-07-14.
//

#include "map.h"
#include <stdlib.h>
#include "extern/stb_ds.h"

#define CHUNK_COORD_HASH(C)
#define CHUNK2AXIAL(C)

Chunk generate_chunk(const Axial coord) {
    // a single map cell is 10x10
    Chunk chunk = {
        .coord = coord,
        .layers = {0}
    };

    // create inlets
    chunk.layers[0] = malloc(sizeof(Tile) * 10 * 10);
    memset(chunk.layers[0], 0, sizeof(Tile) * 10 * 10);

    // for (int i = 0; i < 6; ++i) {
    //     const int a = GetRandomValue(0, 6);
    //     const int d = GetRandomValue(0, CHUNK_RADIUS);
    //     chunk.layers[0][CHUNK_COORD_HASH((ChunkCoord){a,d})] = (Tile) {
    //         .coord = CHUNK2AXIAL((ChunkCoord){a, d}),
    //         .type = TF_CAN_INTERACT | TF_CAN_BUILD
    //     };
    // }

    return chunk;
}

void delete_chunk(const Chunk *chunk) {
    for (int i = 0; i < 5; ++i) {
        if (chunk->layers[i] != nullptr) { free(chunk->layers[i]); }
    }
}

static int GetMapDistance(const Axial a, const Axial b) {
    const Axial coord = AxialSubtract(a, b);
    return (abs(coord.q) + abs(coord.q + coord.r) + abs(coord.r)) / 2;
}

Axial GetMapNeighbor(const Axial coord, const AxialDirection neighborDirection) {
    return AxialAdd(coord, hexDirections[neighborDirection]);
}

Tile * get_chunk_tile(const Chunk *chunk, const Axial coord) {
    // return &map->layers[0][HEX_COORD_INDEX(map, coord)];
    return nullptr;
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