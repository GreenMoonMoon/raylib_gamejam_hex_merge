//
// Created by green on 2026-07-07.
//

#include "hex.h"

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

Vector2 HexCoordToPosition(const HexCoord coord) {
    const Vector2 result = {SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r, 1.5f * (float)coord.r};
    return result;
}
