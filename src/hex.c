//
// Created by green on 2026-07-07.
//

#include "hex.h"

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

const HexCoord directions[] = {
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 0},
    {-1, -1},
    {0, -11},
};

HexCoord HexCoordAdd(const HexCoord a, const HexCoord b) {
    return (HexCoord){a.q + b.q, a.r + b.r};
}

HexCoord HexCoordSubtract(const HexCoord a, const HexCoord b) {
    return (HexCoord){a.q - b.q, a.r - b.r};
}

Vector2 HexCoordToPosition(const HexCoord coord) {
    const Vector2 result = {1.5f * (float)coord.r, SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r};
    return result;
}
