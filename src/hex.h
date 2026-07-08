//
// Created by green on 2026-07-07.
//
// derived from RedBlobGames implementation
// https://www.redblobgames.com/grids/hexagons/implementation.html

#ifndef RAYLIB_GAME_TEMPLATE_HEX_H
#define RAYLIB_GAME_TEMPLATE_HEX_H

#include "raylib.h"

#define GRID_OFFSET_X 0.5f
#define GRID_OFFSET_Y 0.86603f

typedef enum HexDirection {
    HD_NORTH,
    HD_NORTH_EAST,
    HD_SOUTH_EAST,
    HD_SOUTH,
    HD_SOUTH_WEST,
    HD_NORTH_WEST,
} HexDirection;

typedef struct HexGrid {

} HexGrid;

typedef struct HexCoord {
    int q, r;
} HexCoord;

HexCoord HexCoordAdd(HexCoord a, HexCoord b);

HexCoord HexCoordSubtract(HexCoord a, HexCoord b);

Vector2 HexCoordToPosition(HexCoord coord);

#endif //RAYLIB_GAME_TEMPLATE_HEX_H
