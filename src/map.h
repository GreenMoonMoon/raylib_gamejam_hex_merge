//
// Created by green on 2026-07-14.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_MAP_H
#define RAYLIB_GAMEJAM_ENTRY_MAP_H

#include "hex.h"
#include "pipes.h"

#define CHUNK_SIZE 32
#define HALF_CHUNK_SIZE 16

#define MAP_BUCKET_COUNT 1024
#define MAP_CHUNK_WIDTH 256

#define CHECKER2INDEX(C, R) ((C) * HALF_CHUNK_SIZE + (R) / 2)
#define INDEX2CHECKER(I) ((Checker){(I) / HALF_CHUNK_SIZE, (I) % HALF_CHUNK_SIZE * 2 + ((I) / HALF_CHUNK_SIZE) % 2})

// cell flags
#define TF_CAN_INTERACT 0x1
#define TF_CAN_BUILD 0x2
#define TF_SOURCE 0x4
#define TF_BLUEPRINT 0x8
#define TF_OBSTACLE 0xf
#define TF_STACK 0x10

typedef enum MapLayer {
    MAP_LAYER_TERRAIN,
    MAP_LAYER_PIPE,
    MAP_LAYER_BLUEPRINT,

    MAP_LAYER_COUNT
} MapLayer;

typedef struct Tile {
    unsigned int flags;
    int part_id;
    short inputs;
} Tile;

typedef struct Chunk {
    Checker coord;
    Tile *layers[MAP_LAYER_COUNT];

    // static tile mesh cache. for instance rendering
    bool need_blueprint_cache_rebuild;
    int blueprint_mesh_cache_count[PIPE_COUNT];
    MeshTransform *blueprint_mesh_cache[HALF_CHUNK_SIZE * CHUNK_SIZE];
    bool need_mesh_cache_rebuild;
    MeshTransform *tile_mesh_cache[HALF_CHUNK_SIZE * CHUNK_SIZE];
} Chunk;

void init_map();

void close_map();

Chunk generate_chunk(Checker coord);

void delete_chunk(const Chunk *chunk);

Axial GetMapNeighbor(Axial coord, AxialDirection neighborDirection);

Tile *get_chunk_tile(const Chunk *chunk, Axial axial);

bool check_chunk_collision(const Chunk *chunk, Axial coord);

bool is_tile_free(Chunk *chunk, Axial coord);

void commit_pipe_blueprints_to_map(Blueprint *pipe_blueprint_list);

void draw_map();

#endif //RAYLIB_GAMEJAM_ENTRY_MAP_H
