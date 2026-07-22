//
// Created by green on 2026-07-21.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_PIPES_H
#define RAYLIB_GAMEJAM_ENTRY_PIPES_H

#include "raylib.h"
#include "hex.h"

enum PipeModelID {
    PIPE_BEND,
    PIPE_DIR_VALVE_MOUNT,
    PIPE_END,
    PIPE_5SPLIT,
    PIPE_JOINT,
    PIPE_4SPLIT,
    PIPE_SHORT_END,
    PIPE_WELL,
    PIPE_6SPLIT,
    PIPE_STRAIGHT,
    PIPE_3SPLIT,
    PIPE_JOINT_VALVE_MOUNT,
    PIPE_W_SPLIT,
    PIPE_X_SPLIT,
    PIPE_Y_SPLIT,
    PIPE_VALVE,

    PIPE_COUNT
};

void load_pipes_resources();

void unload_pipes_resources();

void draw_pipes();

void draw_pipe_wire(enum PipeModelID id, Vector2 position, int rotation, Color color);

void instantiate_pipe(enum PipeModelID id, Axial coord, int rotation);

const char *get_pipe_name(enum PipeModelID id);

#endif //RAYLIB_GAMEJAM_ENTRY_PIPES_H
