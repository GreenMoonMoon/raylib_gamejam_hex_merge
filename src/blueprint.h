//
// Created by green on 2026-07-17.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_BUILDING_H
#define RAYLIB_GAMEJAM_ENTRY_BUILDING_H

#include "hex.h"

typedef struct Blueprint {
    int id;
    // MeshTransform transform;
    Axial coord;
    char inputs;
} Blueprint;

#endif //RAYLIB_GAMEJAM_ENTRY_BUILDING_H
