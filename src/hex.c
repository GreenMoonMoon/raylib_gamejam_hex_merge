//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

const HexCoord hexDirections[6] = {
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, 0},
    {-1, 1},
    {0, 1},
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
    const Vector2 result = {1.5f * (float)coord.r, SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r};
    return result;
}

HexCoord PositionToHexCoord(const Vector2 position) {
    const float fractional_q = SQRT_3 / 3.0f * position.y - 1.0f / 3.0f * position.x;
    const float fractional_r = 2.0f / 3.0f * position.x;

    return HexCoordRound(fractional_q, fractional_r);
}
