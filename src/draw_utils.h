//
// Created by green on 2026-07-07.
//

#ifndef RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
#define RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H

#include "raylib.h"
#include "hex.h"
#include "map.h"

void DrawHexMapGrid(HexMap map);

void DrawHexGrid(int rows, int columns);

void DrawHex(HexCoord coord, float height, Color color);

void DrawHexWire(HexCoord coord, float height, Color color);

#endif //RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
