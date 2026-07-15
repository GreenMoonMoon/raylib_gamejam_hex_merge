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

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

#define OUT_OF_BOUND(M_ptr, C) ((C).q < 0 || (C).q >= (M_ptr)->sizeQ || (C).r < 0 || (C).r >= (M_ptr)->sizeR)
#define QR_INDEX(M_ptr, Q, R) ((Q) * (M_ptr)->sizeQ + (R))
#define HEX_COORD_INDEX(M_ptr, C) ((C).q * (M_ptr)->sizeQ + (C).r)
#define INDEX_HEX_COORD(M_ptr, I) (HexCoord){(I) / (M_ptr)->sizeQ, (I) % (M_ptr)->sizeQ}

typedef enum HexDirection {
    HD_NORTH,
    HD_NORTH_EAST,
    HD_SOUTH_EAST,
    HD_SOUTH,
    HD_SOUTH_WEST,
    HD_NORTH_WEST,
    HD_COUNT
} HexDirection;


typedef struct HexCoord {
    int q, r;
} HexCoord;

extern const HexCoord hexDirections[];

HexCoord HexCoordAdd(HexCoord a, HexCoord b);

HexCoord HexCoordSubtract(HexCoord a, HexCoord b);

/// Rounding fractional hex coordinate
/// based on Jacob Rus implementation https://observablehq.com/@jrus/hexround
/// @param q fractional q
/// @param r fractional r
/// @return q and r rounded to HexCoord
HexCoord HexCoordRound(float q, float r);

bool HexCoordEqual(HexCoord a, HexCoord b);

Vector2 HexCoordToPosition(HexCoord coord);

HexCoord PositionToHexCoord(Vector2 position);

#endif //RAYLIB_GAME_TEMPLATE_HEX_H
