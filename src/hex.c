//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>
#include <stdlib.h>

// AXIAL COORDINATE

const Axial hexDirections[6] = {
    {-1, 0},
    {-1, 1},
    {0, 1},
    {1, 0},
    {1, -1},
    {0, -1},
};

inline Axial AxialAdd(const Axial a, const Axial b) {
    return (Axial){a.q + b.q, a.r + b.r};
}

inline Axial AxialSubtract(const Axial a, const Axial b) {
    return (Axial){a.q - b.q, a.r - b.r};
}

Axial AxialRound(float q, float r) {
    const float gq = roundf(q);
    const float gr = roundf(r);
    q -= gq;
    r -= gr;
    const float dq = roundf(q + 0.5f * r) * (float)(q * q >= r * r);
    const float dr = roundf(r + 0.5f * q) * (float)(q * q < r * r);
    return (Axial){(int)(gq + dq), (int)(gr + dr)};
}

inline bool AxialEqual(const Axial a, const Axial b) {
    return (a.q == b.q && a.r == b.r);
}

inline int AxialLength(const Axial coord) {
    return (abs(coord.q) + abs(coord.r) + abs(-coord.q-coord.r)) / 2;
}

inline int AxialDistance(const Axial a, const Axial b) {
    return AxialLength(AxialSubtract(a, b));
}

inline Vector2 AxialToPosition(const Axial coord) {
    return (Vector2) {
        1.5f * (float)coord.q,
        SQRT_3_2 * (float)coord.q + SQRT_3 * (float)coord.r
    };
}

Axial PositionToAxial(const Vector2 position) {
    const Vector2 fractional_axial = {
        0.6666667f *  position.x
         -0.3333334f * position.x + SQRT_3_3 * position.y,
    };
    return AxialRound(fractional_axial.x, fractional_axial.y);
}

// CHECKER COORDINATE

inline Checker CheckerAbs(const Checker coord) {
    return (Checker){ abs(coord.col), abs(coord.row)};
}

inline Checker CheckerAdd(const Checker a, const Checker b) {
    return (Checker){a.col + b.col, a.row + b.row};
}

inline Checker CheckerSubtract(const Checker a, const Checker b) {
    return (Checker){a.col - b.col, a.row - b.row};
}

int CheckerDistance(const Checker a, const Checker b) {
    const Checker d = CheckerAbs(CheckerSubtract(a, b));
    const int v = (d.col - d.row) / 2;
    return d.row + (v > 0 ? v : 0);
}

inline Checker PositionToChecker(const Vector2 position) {
    return (Checker) {
        .col = (int)(position.x / 1.5f),
        .row = (int)(position.y / SQRT_3_2)
    };
}

inline Vector2 CheckerToPosition(const Checker coord) {
    return (Vector2) {
        .x = 1.5f * (float)coord.col,
        .y = 2.0f * SQRT_3_2 * (float)coord.row
    };
}

// CONVERSIONS

inline Checker axial_to_checker(const Axial coord) {
    return (Checker){.col = coord.q, .row = 2 * coord.q + coord.r};
}

inline Axial checker_to_axial(const Checker coord) {
    return (Axial){.q = coord.col, .r = (coord.row - coord.col) / 2};
}
