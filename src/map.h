//
// Created by green on 2026-07-14.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_MAP_H
#define RAYLIB_GAMEJAM_ENTRY_MAP_H

#include "hex.h"

#define CHUNK_SIZE 10
#define CHUNK_LAYER_COUNT 1

// cell flags
#define TF_CAN_INTERACT 0x1
#define TF_CAN_BUILD 0x2
#define TF_SOURCE 0x4
#define TF_OBSTACLE 0xf

// #define QR_INDEX(M_ptr, Q, R) ((Q) * (M_ptr)->sizeQ + (R))
// #define HEX_COORD_INDEX(M_ptr, C) ((C).q * (M_ptr)->sizeQ + (C).r)
// #define INDEX_HEX_COORD(M_ptr, I) (HexCoord){(I) / (M_ptr)->sizeQ, (I) % (M_ptr)->sizeQ}

typedef struct Tile {
    unsigned int type;
} Tile;

typedef struct Chunk {
    Checker coord;
    Tile *layers[CHUNK_LAYER_COUNT];
} Chunk;

Chunk generate_chunk(Checker coord);

void delete_chunk(const Chunk *chunk);

Axial GetMapNeighbor(Axial coord, AxialDirection neighborDirection);

Tile *get_chunk_tile(const Chunk *chunk, Axial coord);

bool check_chunk_collision(const Chunk *chunk, Axial coord);

bool is_tile_free(Chunk *chunk, Axial coord);

#endif //RAYLIB_GAMEJAM_ENTRY_MAP_H
