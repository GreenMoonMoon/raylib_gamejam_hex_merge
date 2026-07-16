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

// AXIAL COORDINATE

typedef enum AxialDirection {
    HD_NORTH,
    HD_NORTH_EAST,
    HD_SOUTH_EAST,
    HD_SOUTH,
    HD_SOUTH_WEST,
    HD_NORTH_WEST,
    HD_COUNT
} AxialDirection;

/// Hexagonal Coordinate Axial
typedef struct AxialCoordinate {
    int q, r;
} Axial;

extern const Axial hexDirections[];

Axial AxialAdd(Axial a, Axial b);

Axial AxialSubtract(Axial a, Axial b);

/// Rounding fractional hex coordinate
/// based on Jacob Rus implementation https://observablehq.com/@jrus/hexround
/// @param q fractional q
/// @param r fractional r
/// @return q and r rounded to HexCoord
Axial AxialRound(float q, float r);

bool AxialEqual(Axial a, Axial b);

int AxialLength(Axial coord);

int AxialDistance(Axial a, Axial b);

Vector2 AxialToPosition(Axial coord);

Axial PositionToAxial(Vector2 position);

// CHECKER COORDINATE

/// Hexagonal "doubled" coordinate where vertical step size is doubled
/// more info: https://www.redblobgames.com/grids/hexagons/#line-drawing
typedef struct CheckerCoordinate { // RedBlobGame called it Doubled
    int col, row;
} Checker;

Checker CheckerAbs(Checker coord);

Checker CheckerAdd(Checker a, Checker b);

Checker CheckerSubtract(Checker a, Checker b);

int CheckerDistance(Checker a, Checker b);

Checker PositionToChecker(Vector2 position);

Vector2 CheckerToPosition(Checker coord);

// CONVERSIONS

Checker axial_to_checker(Axial coord);

Axial checker_to_axial(Checker coord);

#endif //RAYLIB_GAME_TEMPLATE_HEX_H
