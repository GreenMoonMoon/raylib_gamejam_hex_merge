//
// Created by green on 2026-07-17.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_BUILDING_H
#define RAYLIB_GAMEJAM_ENTRY_BUILDING_H

typedef enum BuildingType {
    BUILDING_WELL,

    BUILDING_COUNT,
} BuildingType;

const char *building_names[] = {
    "well",
};

typedef struct Building {
    BuildingType type;
} Building;

#endif //RAYLIB_GAMEJAM_ENTRY_BUILDING_H
