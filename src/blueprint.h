//
// Created by green on 2026-07-17.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_BUILDING_H
#define RAYLIB_GAMEJAM_ENTRY_BUILDING_H

#include "raylib.h"

typedef struct MeshTransform {
    Vector3 position;
    float rotation;
} MeshTransform;

typedef struct Blueprint {
    int id;
    // TODO: how to keep resources count...
} Blueprint;

#endif //RAYLIB_GAMEJAM_ENTRY_BUILDING_H
