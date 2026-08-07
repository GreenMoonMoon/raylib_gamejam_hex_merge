//
// Created by green on 2026-07-14.
//

#include "map.h"
#include <stdlib.h>
#include "extern/stb_ds.h"

#define GET_CHUNK_ROOT(C) (checker_multiply(checker_divide(C,(Checker){CHUNK_SIZE,HALF_CHUNK_SIZE}),(Checker){CHUNK_SIZE,HALF_CHUNK_SIZE}))

struct ChunkEntry {
    Checker key;
    Chunk value;
};
static struct ChunkEntry *loaded_chunk_hashmap = nullptr;

static Axial PathNextMapCoordinate(const Chunk *chunk, const Axial from, const Axial to) {
    return from;
}

static void rebuild_chunk_mesh_cache() {
    for (int i = 0; i < hmlen(loaded_chunk_hashmap); ++i) {
        if (loaded_chunk_hashmap[i].value.need_blueprint_cache_rebuild) {

        }
    }
}

static void draw_chunk(const Checker coord, const Chunk chunk) {
    // draw mesh caches
    // blueprint ...
    // pipe ...

    // draw individual tiles
    for (int i = 0; i < HALF_CHUNK_SIZE * CHUNK_SIZE; ++i) {
        const Vector2 position = CheckerToPosition(checker_add(coord, INDEX2CHECKER(i)));
        if ((chunk.layers[MAP_LAYER_TERRAIN][i].flags & TF_SOURCE) != 0) {
            DrawCube((Vector3){.x = position.x, .y = 0, .z = position.y}, 0.5f, 0.5f, 0.5f, RED);
        }
        else if (chunk.layers[MAP_LAYER_TERRAIN][i].flags & TF_CAN_INTERACT) {
            DrawCube((Vector3){.x = position.x, .y = 0, .z = position.y}, 0.5f, 0.5f, 0.5f, GRAY);
        }
    }
}

void init_map() {
}

void close_map() {
    if (nullptr == loaded_chunk_hashmap) { return; }
    for (int i = 0; i < hmlen(loaded_chunk_hashmap); ++i) {
        delete_chunk(&loaded_chunk_hashmap[i].value);
    }
    hmfree(loaded_chunk_hashmap);
}

Chunk generate_chunk(const Checker coord) {
    Chunk chunk = {
        .coord = coord,
        .layers = {0}
    };

    // create inlets
    const size_t msize = sizeof(Tile) * CHUNK_SIZE * HALF_CHUNK_SIZE;
    for (int i = 0; i < MAP_LAYER_COUNT; ++i) {
        chunk.layers[i] = malloc(msize);
        memset(chunk.layers[i], 0, msize);
    }

    chunk.layers[MAP_LAYER_TERRAIN][0].flags |= (TF_STACK | TF_CAN_INTERACT);

    for (int i = 0; i < 6; ++i) {
        const int c = GetRandomValue(0, CHUNK_SIZE);
        const int r = GetRandomValue(0, CHUNK_SIZE) + c % 2;
        const int index = CHECKER2INDEX(c, r);
        if (chunk.layers[MAP_LAYER_TERRAIN][index].flags != 0) { continue; }

        chunk.layers[MAP_LAYER_TERRAIN][CHECKER2INDEX(c, r)] = (Tile) {
            .flags = TF_CAN_INTERACT | TF_CAN_BUILD | TF_SOURCE
        };
    }

    hmput(loaded_chunk_hashmap, coord, chunk);

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
    return false;
}

bool is_tile_free(Chunk *chunk, Axial coord) {
    return true;
}

void commit_pipe_blueprints_to_map(Blueprint *pipe_blueprint_list) {
    for (int i = 0; i < arrlen(pipe_blueprint_list); ++i) {
        const Checker tile_coord = axial_to_checker(pipe_blueprint_list[i].coord);
        const Checker chunk_coord = GET_CHUNK_ROOT(tile_coord);
        const int ci = hmgeti(loaded_chunk_hashmap, chunk_coord);

        if (ci < 0) {
            TraceLog(LOG_ERROR, "somehow a blueprint is trying to be placed on an unloaded chunk. This is not normal!");
            continue;
        }

        loaded_chunk_hashmap[ci].value.layers[MAP_LAYER_BLUEPRINT][CHECKER2INDEX(tile_coord.col, tile_coord.row)] = (Tile){
            .flags = TF_BLUEPRINT,
            .part_id = pipe_blueprint_list[i].id,
            .inputs = pipe_blueprint_list[i].inputs
        };
    }

    rebuild_chunk_mesh_cache();
}

void draw_map() {
    if (nullptr == loaded_chunk_hashmap) {
        TraceLog(LOG_TRACE, "no chunk is currently loaded!");
        return;
    }
    for (int i = 0; i < hmlen(loaded_chunk_hashmap); ++i) {
        draw_chunk(loaded_chunk_hashmap[i].key, loaded_chunk_hashmap[i].value);
    }
}
