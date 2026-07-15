//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>
#include <stdlib.h>

const HCAxial hexDirections[6] = {
    {-1, 0},
    {-1, 1},
    {0, 1},
    {1, 0},
    {1, -1},
    {0, -1},
};

HCAxial HCAAdd(const HCAxial a, const HCAxial b) {
    return (HCAxial){a.q + b.q, a.r + b.r};
}

HCAxial HCASubtract(const HCAxial a, const HCAxial b) {
    return (HCAxial){a.q - b.q, a.r - b.r};
}

HCAxial HCARound(float q, float r) {
    const float gq = roundf(q);
    const float gr = roundf(r);
    q -= gq;
    r -= gr;
    const float dq = roundf(q + 0.5f * r) * (float)(q * q >= r * r);
    const float dr = roundf(r + 0.5f * q) * (float)(q * q < r * r);
    return (HCAxial){(int)(gq + dq), (int)(gr + dr)};
}

inline bool HCAEqual(const HCAxial a, const HCAxial b) {
    return (a.q == b.q && a.r == b.r);
}

int HCALength(const HCAxial coord) {
    return (abs(coord.q) + abs(coord.r) + abs(-coord.q-coord.r)) / 2;
}

int HCADistance(const HCAxial a, const HCAxial b) {
    return HCALength(HCASubtract(a, b));
}

Vector2 HCAToPosition(const HCAxial coord) {
    const Vector2 result = {
        1.5f * (float)coord.r + GRID_OFFSET_X,
        SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r + GRID_OFFSET_Y
    };
    return result;
}

HCAxial PositionToHCA(const Vector2 position) {
    const Vector2 pos = {position.x - GRID_OFFSET_X, position.y - GRID_OFFSET_Y};
    const float fractional_q = SQRT_3 / 3.0f * pos.y - 1.0f / 3.0f * pos.x;
    const float fractional_r = 2.0f / 3.0f * pos.x;

    return HCARound(fractional_q, fractional_r);
}