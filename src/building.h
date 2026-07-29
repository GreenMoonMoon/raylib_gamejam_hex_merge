//
// Created by green on 2026-07-17.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_BUILDING_H
#define RAYLIB_GAMEJAM_ENTRY_BUILDING_H

#include "pipes.h"

typedef struct Blueprint {
    PipeBlueprint pipes;
} Blueprint;

Blueprint create_blueprint();

void delete_blueprint(Blueprint *blueprint);

#endif //RAYLIB_GAMEJAM_ENTRY_BUILDING_H
