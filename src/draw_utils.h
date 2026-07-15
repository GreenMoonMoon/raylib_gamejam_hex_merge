//
// Created by green on 2026-07-07.
//

#ifndef RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
#define RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H

#include "raylib.h"
#include "hex.h"
#include "map.h"

void DrawHexMapGrid(Chunk map);

void DrawHexGrid(int rows, int columns);

void DrawHex(HCAxial coord, float height, Color color);

void DrawHexWire(HCAxial coord, float height, Color color);

void DrawChunkBoundaries(HCAxial coord, float height, Color color);

#endif //RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
