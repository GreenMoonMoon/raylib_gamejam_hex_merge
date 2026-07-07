//
// Created by green on 2026-07-07.
//

#include "draw_utils.h"
#include "raylib.h"
#include "rlgl.h"

static void DrawHexCell(const float x, const float y) {
    // normalized flat hex cell corners
    const float coordinate[6 * 2] = {
        -1.0f, 0, -0.5f, 0.86603f, 0.5f, 0.86603f, 1.0f, 0, 0.5f, -0.86603f, -0.5f, -0.86603f
    };

    for (int i = 0; i < 5; ++i) {
        rlVertex3f(x + coordinate[i * 2], 0, y + coordinate[i * 2 + 1]);
        rlVertex3f(x + coordinate[i * 2 + 2], 0, y + coordinate[i * 2 + 3]);
    }
    rlVertex3f(x + coordinate[10], 0, y + coordinate[11]);
    rlVertex3f(x + coordinate[0], 0, y + coordinate[1]);
}

void DrawHexGrid(const int rows, const int columns) {
#define CELL_HEIGHT 0.86603f

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

            DrawHexCell(x, y);
        }
    }

    rlEnd();
    rlPopMatrix();
}

