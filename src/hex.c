//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>
#include <stdlib.h>
#include "extern/stb_ds.h"

const HexCoord hexDirections[6] = {
    {-1, 0},
    {-1, 1},
    {0, 1},
    {1, 0},
    {1, -1},
    {0, -1},
};

HexCoord HexCoordAdd(const HexCoord a, const HexCoord b) {
    return (HexCoord){a.q + b.q, a.r + b.r};
}

HexCoord HexCoordSubtract(const HexCoord a, const HexCoord b) {
    return (HexCoord){a.q - b.q, a.r - b.r};
}

HexCoord HexCoordRound(float q, float r) {
    const float gq = roundf(q);
    const float gr = roundf(r);
    q -= gq;
    r -= gr;
    const float dq = roundf(q + 0.5f * r) * (float)(q * q >= r * r);
    const float dr = roundf(r + 0.5f * q) * (float)(q * q < r * r);
    return (HexCoord){(int)(gq + dq), (int)(gr + dr)};
}

inline bool HexCoordEqual(const HexCoord a, const HexCoord b) {
    return (a.q == b.q && a.r == b.r);
}

Vector2 HexCoordToPosition(const HexCoord coord) {
    const Vector2 result = {
        1.5f * (float)coord.r + GRID_OFFSET_X,
        SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r + GRID_OFFSET_Y
    };
    return result;
}

HexCoord PositionToHexCoord(const Vector2 position) {
    const Vector2 pos = {position.x - GRID_OFFSET_X, position.y - GRID_OFFSET_Y};
    const float fractional_q = SQRT_3 / 3.0f * pos.y - 1.0f / 3.0f * pos.x;
    const float fractional_r = 2.0f / 3.0f * pos.x;

    return HexCoordRound(fractional_q, fractional_r);
}