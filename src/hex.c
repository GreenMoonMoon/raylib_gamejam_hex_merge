//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>
#include <stdlib.h>

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

#define OUT_OF_BOUND(C) ((C).q < 0 || (C).q > currentMap->sizeQ || (C).r < 0 || (C).r > currentMap->sizeR)
#define HEX_COORD_INDEX(C) ((C).q * currentMap->sizeQ + (C).r)

HexMap *currentMap;

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
    const Vector2 result = {1.5f * (float)coord.r, SQRT_3 * (float)coord.q + SQRT_3_2 * (float)coord.r};
    return result;
}

HexCoord PositionToHexCoord(const Vector2 position) {
    const float fractional_q = SQRT_3 / 3.0f * position.y - 1.0f / 3.0f * position.x;
    const float fractional_r = 2.0f / 3.0f * position.x;

    return HexCoordRound(fractional_q, fractional_r);
}

static int GetMapDistance(const HexCoord a, const HexCoord b) {
    const HexCoord coord = HexCoordSubtract(a, b);
    return (abs(coord.q) + abs(coord.q + coord.r) + abs(coord.r)) / 2;
}

HexCoord GetMapNeighbor(const HexCoord coord, const HexDirection neighborDirection) {
    return HexCoordAdd(coord, hexDirections[neighborDirection]);
}

bool CheckMapCollision(const HexCoord coord) {
    if (OUT_OF_BOUND(coord)) { return true; }
    return currentMap->cells[HEX_COORD_INDEX(coord)] != 0;
}

HexCoord PathNextMapCoordinate(const HexCoord from, const HexCoord to) {
    struct Cell {
        HexCoord coord;
        int index;
    };

    struct CameFrom {
        int from;
        bool visited;
    };

    // frontier uses the index as the cameFrom cell index
    struct Cell frontier[currentMap->sizeQ * currentMap->sizeR / 3]; // arbitrary number I'm (almost) sure can't be reached
    // cameFrom uses the index for the cell visited before
    struct Cell cameFrom[currentMap->sizeQ * currentMap->sizeR];

    int ci = 0;
    cameFrom[ci] = (struct Cell){from, 0}; // initial cell
    int fi = 0;
    frontier[fi++] = (struct Cell){from, ci};
    ci++;

    bool foundPath = false;
    while (fi > 0) {
        const struct Cell current = frontier[--fi];
        if (HexCoordEqual(current.coord, to)) {
            foundPath = true;
            break;
        }

        for (int i = 0; i < HD_COUNT; ++i) {
            const HexCoord neighbor = GetMapNeighbor(current.coord, i);
            if (OUT_OF_BOUND(neighbor)) { continue; }

            bool isReached = false;
            for (int j = 0; j < ci; ++j) {
                if (HexCoordEqual(cameFrom[j].coord, neighbor)) {
                    isReached = true;
                    break;
                }
            }

            if (!isReached) {
                cameFrom[ci] = (struct Cell){neighbor, current.index};
                frontier[fi++] = (struct Cell){neighbor, ci};
                ci++;
            }
        }
    }

    // reconstruct the path from the target
    struct Cell cf = cameFrom[ci - 1];
    if (foundPath) {
        while (cf.index != 0) {
            cf = cameFrom[cf.index];
        }
        return cf.coord;
    }
    return from;
}