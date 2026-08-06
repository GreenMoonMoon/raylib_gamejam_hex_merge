//
// Created by green on 2026-07-14.
//

#include "map.h"

#include <stdlib.h>
#include "extern/stb_ds.h"

#define BUCKET_HASH(C) (((C).row * CHUNK_SIZE + (C).col) % MAP_BUCKET_COUNT)

// dense array
static Chunk *loaded_chunk_lists[MAP_BUCKET_COUNT];

static Axial PathNextMapCoordinate(const Chunk *chunk, const Axial from, const Axial to) {
    return from;
}

void init_map() {
    // initialize every bucket to nullptr to work with stb_ds
    for (int i = 0; i < MAP_BUCKET_COUNT; ++i) { loaded_chunk_lists[i] = nullptr; }
}

void close_map() {

}

Chunk generate_chunk(const Checker coord) {
    Chunk chunk = {
        .coord = coord,
        .layers = {0}
    };

    // create inlets
    const size_t msize = sizeof(Tile) * CHUNK_SIZE * (CHUNK_SIZE * 2);
    chunk.layers[0] = malloc(msize);
    memset(chunk.layers[0], 0, msize);

    chunk.layers[0][0].flags |= (TF_STACK | TF_CAN_INTERACT);

    for (int i = 0; i < 6; ++i) {
        const int c = GetRandomValue(0, CHUNK_SIZE);
        const int r = GetRandomValue(0, CHUNK_SIZE) + c % 2;
        const int index = CHECKER2INDEX(c, r);
        if (chunk.layers[0][index].flags != 0) { continue; }

        chunk.layers[0][CHECKER2INDEX(c, r)] = (Tile) {
            .flags = TF_CAN_INTERACT | TF_CAN_BUILD | TF_SOURCE
        };
    }

    arrput(loaded_chunk_lists[BUCKET_HASH(coord)], chunk);

    return chunk;
}

void delete_chunk(const Chunk *chunk) {
    for (int i = 0; i < MAP_LAYER_COUNT; ++i) {
        if (chunk->layers[i] != nullptr) { free(chunk->layers[i]); }
    }
}

Axial GetMapNeighbor(const Axial coord, const AxialDirection neighborDirection) {
    return AxialAdd(coord, hexDirections[neighborDirection]);
}

Tile * get_chunk_tile(const Chunk *chunk, const Axial axial) {
    const Checker coord = axial_to_checker(axial);
    const unsigned int index = CHECKER2INDEX(coord.col, coord.row);
    if (index >= CHUNK_SIZE * CHUNK_SIZE) { return nullptr; }
    return &chunk->layers[0][index];
}

bool check_chunk_collision(const Chunk *chunk, const Axial coord) {
    // if (OUT_OF_BOUND(map, coord)) { return true; }
    return false;
}

bool is_tile_free(Chunk *chunk, Axial coord) {
    return true;
}

static void draw_chunk(const Chunk chunk) {
    for (int c = 0; c < CHUNK_SIZE; ++c) {
        for (int r = c % 2; r < CHUNK_SIZE; r += 2) {
            // draw tiles...

            // draw blueprints...

            const Tile tile = chunk.layers[0][CHECKER2INDEX(c, r)];
            if (tile.flags & TF_STACK) {
                const Vector2 position = CheckerToPosition((Checker){.col = c, .row = r});
                DrawCube((Vector3){.x = position.x, .y = 0, .z = position.y}, 0.5f, 0.5f, 0.5f, GRAY);
            } else if (tile.flags & TF_CAN_INTERACT) {
                const Vector2 position = CheckerToPosition((Checker){.col = c, .row = r});
                DrawCube((Vector3){.x = position.x, .y = 0, .z = position.y}, 0.5f, 0.5f, 0.5f, RED);
            }
        }
    }
}

void draw_map() {
    for (int i = 0; i < MAP_BUCKET_COUNT; ++i) {
        if (nullptr == loaded_chunk_lists[i]) { continue; }
        for (int j = 0; j < arrlen(loaded_chunk_lists[i]); ++j) {
            draw_chunk(loaded_chunk_lists[i][j]);
        }
    }
}
