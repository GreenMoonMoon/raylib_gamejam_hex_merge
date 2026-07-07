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

typedef struct HexGrid {

} HexGrid;

typedef struct HexCoord {
    int q, r;
} HexCoord;

Vector2 HexCoordToPosition(HexCoord coord);

#endif //RAYLIB_GAME_TEMPLATE_HEX_H
