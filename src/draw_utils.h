//
// Created by green on 2026-07-07.
//

#ifndef RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
#define RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H

#include "raylib.h"
#include "hex.h"
#include "map.h"

void DrawHexGrid(int rows, int columns);

void DrawHex(Axial coord, float height, Color color);

void DrawHexWire(Axial coord, float height, Color color);

void DrawChunkBoundaries(Checker coord, float height, Color color);

void draw_chunk_grid(Chunk chunk, Color color);

void draw_mesh_wire(Mesh mesh, Material material, Matrix transform);

#endif //RAYLIB_GAME_TEMPLATE_DRAW_UTILS_H
