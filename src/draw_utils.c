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

static void draw_hex_wire(const float x, const float y) {
    // normalized flat hex cell corners
    for (int i = 0; i < 5; ++i) {
        rlVertex3f(x + point_coords[i].x, 0, y + point_coords[i].y);
        rlVertex3f(x + point_coords[i + 1].x, 0, y + point_coords[i + 1].y);
    }
    rlVertex3f(x + point_coords[5].x, 0, y + point_coords[5].y);
    rlVertex3f(x + point_coords[0].x, 0, y + point_coords[0].y);
}

static void draw_hex_polygons(const float x, const float y) {
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

            draw_hex_wire(x, y);
        }
    }

    rlEnd();
    rlPopMatrix();
}

void DrawHex(const Axial coord, const float height, const Color color) {
    rlPushMatrix();
    rlTranslatef(0, height, 0);
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    const Vector2 position = AxialToPosition(coord);
    draw_hex_polygons(position.x, position.y);

    rlEnd();
    rlPopMatrix();
}

void DrawHexWire(const Axial coord, const float height, const Color color) {
    rlPushMatrix();
    rlTranslatef(0, height, 0);
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    const Vector2 position = AxialToPosition(coord);
    draw_hex_wire(position.x, position.y);

    rlEnd();
    rlPopMatrix();
}

void DrawChunkBoundaries(const Checker coord, const float height, const Color color) {
    rlPushMatrix();
    rlTranslatef(0, height, 0);

    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    const float hsize = (float)CHUNK_SIZE;
    const Vector2 n  = CheckerToPosition((Checker){coord.col - hsize, coord.row - hsize});
    const Vector2 ne = CheckerToPosition((Checker){coord.col - hsize, coord.row + hsize});
    const Vector2 se = CheckerToPosition((Checker){coord.col + hsize, coord.row + hsize});
    const Vector2 s  = CheckerToPosition((Checker){coord.col + hsize, coord.row - hsize});

    rlVertex3f(n.x, 0, n.y);
    rlVertex3f(ne.x, 0, ne.y);

    rlVertex3f(ne.x, 0, ne.y);
    rlVertex3f(se.x, 0, se.y);

    rlVertex3f(se.x, 0, se.y);
    rlVertex3f(s.x, 0, s.y);

    rlVertex3f(s.x, 0, s.y);
    rlVertex3f(n.x, 0, n.y);

    rlEnd();
    rlPopMatrix();
}

void draw_chunk_grid(const Chunk chunk, const Color color) {
    rlPushMatrix();
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    // // const Checker origin = CheckerSubtract(chunk.coord, (Checker){CHUNK_SIZE, CHUNK_SIZE / 2});
    // for (int r = 0; r < CHUNK_SIZE; ++r) {
    //     for (int c = 0; c < CHUNK_SIZE * 2; ++c) {
    //         rlColor4ub(127, 0, 0, 255);
    //         // const Vector2 pos = AxialToPosition(checker_to_axial(CheckerAdd(origin, (Checker){c, r})));
    //         const Vector2 pos = AxialToPosition(checker_to_axial((Checker){c, r}));
    //         draw_hex_wire(pos.x, pos.y);
    //     }
    // }

    for (int r = 0; r < CHUNK_SIZE; ++r) {
        for (int q = 0; q < CHUNK_SIZE; ++q) {
            // rlColor4ub(0, 127, 0, 255);
            // const Vector2 pos = CheckerToPosition(axial_to_checker((Axial){q, r}));
            // draw_hex_wire(pos.x, pos.y);
            rlColor4ub(127, 0, 0, 255);
            const Vector2 p = AxialToPosition((Axial){q, r});
            draw_hex_wire(p.x, p.y);
        }
    }

    rlEnd();
    rlPopMatrix();
}
