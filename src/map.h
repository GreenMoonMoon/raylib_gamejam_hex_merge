//
// Created by green on 2026-07-14.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_MAP_H
#define RAYLIB_GAMEJAM_ENTRY_MAP_H

#include "hex.h"

#define CELLTYPE_CAN_INTERACT 0x1
#define CELLTYPE_CAN_BUILD 0x2
#define CELLTYPE_SOURCE 0x4
#define CELLTYPE_OBSTACLE 0xf

typedef struct HexCell {
    HexCoord coord;
    unsigned int type;
} HexCell;

// TODO: rename to HexChunk?
typedef struct HexMap {
    int sizeQ;
    int sizeR;

    HexCell *layers[5];
} HexMap;
extern HexMap *currentMap;

HexMap generate_map(void);

void delete_map(HexMap *map);

HexCoord GetMapNeighbor(HexCoord coord, HexDirection neighborDirection);

HexCell *GetMapCell(const HexMap *map, HexCoord coord);

/// @param coord cell coordinate to test
/// @return true if there is a collision
bool CheckMapCollision(const HexMap *map, HexCoord coord);

// HexCoord PathNextMapCoordinate(HexCoord from, HexCoord to);

bool IsMapCellFree(HexMap *map, HexCoord coord);

#endif //RAYLIB_GAMEJAM_ENTRY_MAP_H
