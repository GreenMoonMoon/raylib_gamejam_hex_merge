//
// Created by green on 2026-07-14.
//

#include "map.h"
#include <stdlib.h>
#include "extern/stb_ds.h"

HexMap *currentMap;

HexMap generate_map(void) {
    // a single map cell is 10x10
    HexMap map = {
        .sizeQ = 10,
        .sizeR = 10,
        .layers = {0}
    };

    // create inlets
    map.layers[0] = malloc(sizeof(HexCell) * 10 * 10);
    memset(map.layers[0], 0, sizeof(HexCell) * 10 * 10);
    for (int i = 0; i < 6; ++i) {
        const int q = GetRandomValue(1, 9);
        const int r = GetRandomValue(1, 9);
        map.layers[0][QR_INDEX(&map, q, r)] = (HexCell) {
            .coord = {q, r},
            .type = CELLTYPE_CAN_INTERACT | CELLTYPE_CAN_BUILD
        };
    }

    return map;
}

void delete_map(HexMap *map) {

}

static int GetMapDistance(const HexCoord a, const HexCoord b) {
    const HexCoord coord = HexCoordSubtract(a, b);
    return (abs(coord.q) + abs(coord.q + coord.r) + abs(coord.r)) / 2;
}

HexCoord GetMapNeighbor(const HexCoord coord, const HexDirection neighborDirection) {
    return HexCoordAdd(coord, hexDirections[neighborDirection]);
}

HexCell * GetMapCell(HexMap *map, HexCoord coord) {

}

bool CheckMapCollision(const HexMap *map, const HexCoord coord) {
    if (OUT_OF_BOUND(map, coord)) { return true; }
    return false;
}

bool IsMapCellFree(HexMap *map, HexCoord coord) {
    return true;
}

HexCoord PathNextMapCoordinate(const HexMap *map, const HexCoord from, const HexCoord to) {
    struct Cell {
        int from;
        bool visited;
    };

    int frontier[currentMap->sizeQ * currentMap->sizeR]; // arbitrary number I'm (almost) sure can't be reached
    int fi = 0;
    struct Cell *cameFrom = calloc(currentMap->sizeQ * currentMap->sizeR, sizeof(struct Cell));

    const int toIndex = HEX_COORD_INDEX(map, to);
    const int fromIndex = HEX_COORD_INDEX(map, from);

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

        const HexCoord currentCoord = INDEX_HEX_COORD(map, current);
        for (int i = HD_COUNT - 1; i >= 0; --i) {
            const HexCoord neighbor = GetMapNeighbor(currentCoord, i);
            if (OUT_OF_BOUND(map, neighbor)) { continue; }

            const int neighborIndex = HEX_COORD_INDEX(map, neighbor);
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
        return INDEX_HEX_COORD(map, prevIndex);
    }

    free(cameFrom);
    return from;
}
