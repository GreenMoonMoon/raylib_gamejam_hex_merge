//
// Created by green on 2026-07-14.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_MAP_H
#define RAYLIB_GAMEJAM_ENTRY_MAP_H

#include "hex.h"

typedef struct HexMapCell {
    HexCoord coord;
    int type;
} HexMapCell;

#define CELLTYPE_SOURCE 0x01
#define CELLTYPE_OBSTACLE 0x0f

typedef struct HexMap {
    int sizeQ;
    int sizeR;

    HexMapCell *layers[5];
    // HexMapCell *cells;
} HexMap;
extern HexMap *currentMap;

HexMap generate_map(void);

void delete_map(HexMap *map);

HexCoord GetMapNeighbor(HexCoord coord, HexDirection neighborDirection);

/// @param coord cell coordinate to test
/// @return true if there is a collision
bool CheckMapCollision(HexCoord coord);

// HexCoord PathNextMapCoordinate(HexCoord from, HexCoord to);

bool IsMapCellFree(HexMap *map, HexCoord coord);

#endif //RAYLIB_GAMEJAM_ENTRY_MAP_H
