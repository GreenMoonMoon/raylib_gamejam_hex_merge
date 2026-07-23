//
// Created by green on 2026-07-17.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_BUILDING_H
#define RAYLIB_GAMEJAM_ENTRY_BUILDING_H

struct BPP;

typedef struct Blueprint {
    struct BPP *bpp_list;
} Blueprint;

Blueprint create_blueprint();

void delete_blueprint(Blueprint *blueprint);

#endif //RAYLIB_GAMEJAM_ENTRY_BUILDING_H
