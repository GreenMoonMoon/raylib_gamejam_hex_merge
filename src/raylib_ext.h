//
// Created by green on 2026-08-06.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_RAYLIB_EXT_H
#define RAYLIB_GAMEJAM_ENTRY_RAYLIB_EXT_H

#include "raylib.h"

typedef struct MeshTransform {
    Vector3 position;
    float rotation;
} MeshTransform;

typedef struct InstanceMaterialLocations {
    int position_loc;
    int rotation_loc;
} InstanceMaterialLocations;

void draw_tile_mesh_instances(Mesh mesh, Material material, InstanceMaterialLocations locations, MeshTransform *transforms, int count);

#endif //RAYLIB_GAMEJAM_ENTRY_RAYLIB_EXT_H
