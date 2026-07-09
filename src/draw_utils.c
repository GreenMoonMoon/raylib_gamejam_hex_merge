//
// Created by green on 2026-07-07.
//

#include "draw_utils.h"
#include "rlgl.h"

#define CELL_HEIGHT 0.86603f

const Vector2 point_coords[6] = {
    {-1.0f, 0},
    {-0.5f, 0.86603f},
    {0.5f, 0.86603f},
    {1.0f, 0},
    {0.5f, -0.86603f},
    {-0.5f, -0.86603f}
};

static void rlHexWire(const float x, const float y) {
    // normalized flat hex cell corners
    for (int i = 0; i < 5; ++i) {
        rlVertex3f(x + point_coords[i].x, 0, y + point_coords[i].y);
        rlVertex3f(x + point_coords[i + 1].x, 0, y + point_coords[i + 1].y);
    }
    rlVertex3f(x + point_coords[5].x, 0, y + point_coords[5].y);
    rlVertex3f(x + point_coords[0].x, 0, y + point_coords[0].y);
}

static void rlHexPolygons(const float x, const float y) {
    rlVertex3f(x + point_coords[0].x, 0, y + point_coords[0].y);
    rlVertex3f(x + point_coords[1].x, 0, y + point_coords[1].y);
    rlVertex3f(x + point_coords[2].x, 0, y + point_coords[2].y);

    rlVertex3f(x + point_coords[2].x, 0, y + point_coords[2].y);
    rlVertex3f(x + point_coords[5].x, 0, y + point_coords[5].y);
    rlVertex3f(x + point_coords[0].x, 0, y + point_coords[0].y);

    rlVertex3f(x + point_coords[2].x, 0, y + point_coords[2].y);
    rlVertex3f(x + point_coords[3].x, 0, y + point_coords[3].y);
    rlVertex3f(x + point_coords[5].x, 0, y + point_coords[5].y);

    rlVertex3f(x + point_coords[3].x, 0, y + point_coords[3].y);
    rlVertex3f(x + point_coords[4].x, 0, y + point_coords[4].y);
    rlVertex3f(x + point_coords[5].x, 0, y + point_coords[5].y);
}

void DrawHexMapGrid(const HexMap map) {
    rlPushMatrix();
    rlBegin(RL_LINES);
    rlColor4ub(0, 0, 0, 255);

    for (int r = 0; r < map.sizeR; ++r) {
        for (int q = 0; q < map.sizeQ; ++q) {
            if (map.cells[q * map.sizeQ + r] > 0) { continue; }
            const Vector2 pos = HexCoordToPosition((HexCoord){q, r});
            rlHexWire(pos.x + GRID_OFFSET_X, pos.y + GRID_OFFSET_Y);
        }
    }

    rlEnd();

    rlBegin(RL_TRIANGLES);
    for (int r = 0; r < map.sizeR; ++r) {
        for (int q = 0; q < map.sizeQ; ++q) {
            if (map.cells[q * map.sizeQ + r] != 1) { continue; }
            const Vector2 pos = HexCoordToPosition((HexCoord){q, r});
            rlHexPolygons(pos.x + GRID_OFFSET_X, pos.y + GRID_OFFSET_Y);
        }
    }
    rlEnd();
    rlPopMatrix();
}

void DrawHexGrid(const int rows, const int columns) {
    rlPushMatrix();
    rlBegin(RL_LINES);
    rlColor4ub(0, 0, 0, 255);

    const float half_width = (float)columns / 2.0f * 3.0f - 0.5f;
    const float half_height = (float)rows / 2.0f - 0.5f;

    for (int c = 0; c < columns; ++c) {
        for (int r = 0; r < rows; ++r) {
            const float row_offset = (float)(r % 2) * 1.5f;
            const float x = (float)c * 3 - half_width + row_offset;
            const float y = (float)r * CELL_HEIGHT - half_height;

            rlHexWire(x, y);
        }
    }

    rlEnd();
    rlPopMatrix();
}

void DrawHex(const HexCoord coord, const float height, const Color color) {
    rlPushMatrix();
    rlTranslatef(0, height, 0);
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    const Vector2 position = HexCoordToPosition(coord);
    rlHexPolygons(position.x + GRID_OFFSET_X, position.y + GRID_OFFSET_Y);

    rlEnd();
    rlPopMatrix();
}

void DrawHexWire(const HexCoord coord, const float height, const Color color) {
    rlPushMatrix();
    rlTranslatef(0, height, 0);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    const Vector2 position = HexCoordToPosition(coord);
    rlHexWire(position.x + GRID_OFFSET_X, position.y + GRID_OFFSET_Y);

    rlEnd();
    rlPopMatrix();
}