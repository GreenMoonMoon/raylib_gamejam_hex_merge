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

typedef enum HexDirection {
    HD_NORTH,
    HD_NORTH_EAST,
    HD_SOUTH_EAST,
    HD_SOUTH,
    HD_SOUTH_WEST,
    HD_NORTH_WEST,
    HD_COUNT
} HexDirection;

/// Hexagonal Coordinate Axial
typedef struct HCAxial {
    int q, r;
} HCAxial;

extern const HCAxial hexDirections[];

HCAxial HCAAdd(HCAxial a, HCAxial b);

HCAxial HCASubtract(HCAxial a, HCAxial b);

/// Rounding fractional hex coordinate
/// based on Jacob Rus implementation https://observablehq.com/@jrus/hexround
/// @param q fractional q
/// @param r fractional r
/// @return q and r rounded to HexCoord
HCAxial HCARound(float q, float r);

bool HCAEqual(HCAxial a, HCAxial b);

int HCALength(HCAxial coord);

int HCADistance(HCAxial a, HCAxial b);

Vector2 HCAToPosition(HCAxial coord);

HCAxial PositionToHCA(Vector2 position);

#endif //RAYLIB_GAME_TEMPLATE_HEX_H
