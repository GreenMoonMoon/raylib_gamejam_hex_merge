//
// Created by green on 2026-07-07.
//

#include "hex.h"
#include <math.h>
#include <stdlib.h>
#include "extern/stb_ds.h"

#define SQRT_3 1.73205080757f
#define SQRT_3_2 0.866025403785f

#define OUT_OF_BOUND(C) ((C).q < 0 || (C).q > currentMap->sizeQ || (C).r < 0 || (C).r > currentMap->sizeR)
#define HEX_COORD_INDEX(C) ((C).q * currentMap->sizeQ + (C).r)
#define INDEX_HEX_COORD(I) (HexCoord){(I) / currentMap->sizeQ, (I) % currentMap->sizeQ}

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
        int from;
        bool visited;
    };

    int frontier[currentMap->sizeQ * currentMap->sizeR]; // arbitrary number I'm (almost) sure can't be reached
    int fi = 0;
    struct Cell *cameFrom = calloc(currentMap->sizeQ * currentMap->sizeR, sizeof(struct Cell));

    const int toIndex = HEX_COORD_INDEX(to);
    const int fromIndex = HEX_COORD_INDEX(from);

    int current = fromIndex;
    cameFrom[current] = (struct Cell){fromIndex, true}; // initial cell
    frontier[fi++] = current;

    bool foundPath = false;
    while (fi > 0) {
        current = frontier[--fi];
        if (current == toIndex) {
            foundPath = true;
            break;
        }

        const HexCoord currentCoord = INDEX_HEX_COORD(current);
        for (int i = HD_COUNT - 1; i >= 0; --i) {
            const HexCoord neighbor = GetMapNeighbor(currentCoord, i);
            if (OUT_OF_BOUND(neighbor)) { continue; }

            const int neighborIndex = HEX_COORD_INDEX(neighbor);
            if (!cameFrom[neighborIndex].visited) {
                cameFrom[neighborIndex].visited = true;
                cameFrom[neighborIndex].from = current;
                frontier[fi++] = neighborIndex;
            }
        }
    }

    if (foundPath) {
        int prevIndex = cameFrom[current].from;
        while (prevIndex != fromIndex) {
            prevIndex = cameFrom[prevIndex].from;
        }
        free(cameFrom);
        return INDEX_HEX_COORD(prevIndex);
    }

    free(cameFrom);
    return from;
}