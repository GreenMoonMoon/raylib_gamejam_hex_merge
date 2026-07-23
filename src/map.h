//
// Created by green on 2026-07-14.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_MAP_H
#define RAYLIB_GAMEJAM_ENTRY_MAP_H

#include "hex.h"

#define CHUNK_SIZE 16
#define HALF_CHUNK_SIZE 8
#define CHUNK_LAYER_COUNT 1

#define CHECKER2INDEX(C, R) ((C) * HALF_CHUNK_SIZE + (R) / 2)
#define INDEX2CHECKER(I) ((Checker){(I) / HALF_CHUNK_SIZE, (I) % HALF_CHUNK_SIZE * 2})

// cell flags
#define TF_CAN_INTERACT 0x1
#define TF_CAN_BUILD 0x2
#define TF_SOURCE 0x4
#define TF_BLUEPRINT 0x8
#define TF_OBSTACLE 0xf

typedef struct Tile {
    unsigned int flags;
} Tile;

typedef struct Chunk {
    Checker coord;
    Tile *layers[CHUNK_LAYER_COUNT];
} Chunk;

Chunk generate_chunk(Checker coord);

void delete_chunk(const Chunk *chunk);

Axial GetMapNeighbor(Axial coord, AxialDirection neighborDirection);

Tile *get_chunk_tile(const Chunk *chunk, Checker coord);

bool check_chunk_collision(const Chunk *chunk, Axial coord);

bool is_tile_free(Chunk *chunk, Axial coord);

#endif //RAYLIB_GAMEJAM_ENTRY_MAP_H
