//
// Created by green on 2026-07-21.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_PIPES_H
#define RAYLIB_GAMEJAM_ENTRY_PIPES_H

#include "raylib.h"
#include "hex.h"
#include "building.h"

enum PipeModelID {
    PIPE_BEND,
    PIPE_JOINT,
    PIPE_JOINT_VALVE_MOUNT,
    PIPE_WELL_OPEN,
    PIPE_SHORT_BEND,
    PIPE_SHORT_END,
    PIPE_SPLIT5,
    PIPE_SPLIT_BRANCH,
    PIPE_SPLIT_K,
    PIPE_SPLIT_STAR,
    PIPE_SPLIT_W,
    PIPE_SPLIT_X,
    PIPE_SPLIT_Y,
    PIPE_STRAIGHT,
    PIPE_VALVE,
    PIPE_VALVE_MOUNT,
    PIPE_WELL_CONNECTED,

    PIPE_COUNT,
    PIPE_NONE
};

void load_pipes_resources();

void unload_pipes_resources();

void draw_pipes();

void draw_pipe_wire(enum PipeModelID id, Vector2 position, char rotation, Color color);

void instantiate_pipe(enum PipeModelID id, Axial coord, char rotation);

const char *get_pipe_name(enum PipeModelID id);

void start_pipe_tool(Axial start_tile, char hex_direction);

bool update_pipe_tool(Axial next_tile);

Blueprint commit_pipe_tool();

void draw_pipe_tool();

void draw_pipe_blueprint(Blueprint blueprint);

#endif //RAYLIB_GAMEJAM_ENTRY_PIPES_H
