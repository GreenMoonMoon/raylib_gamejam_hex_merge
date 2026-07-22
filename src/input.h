//
// Created by green on 2026-07-08.
//

#ifndef RAYLIB_GAME_TEMPLATE_INPUT_H
#define RAYLIB_GAME_TEMPLATE_INPUT_H

#include "raylib.h"
#include "hex.h"

typedef enum InputState {
    IS_NONE,
    IS_TOUCH_DRAG,
    IS_TOUCH_SELECT,
    IS_KEYBOARD_DPAD,
} InputState;

typedef struct Inputs {
    Axial selected_tile;
    Vector2 move_vector;
    int h, v;

    bool interact_select;
    bool close;
    bool toggle_build;
    bool rotate;
} Inputs;

// TODO: separate between character inputs and menu inputs as a way to group relevant inputs together and process axial
//  inputs but otherwise just query the buttons state directly.
void ProcessInputs(Inputs *inputs);

#endif //RAYLIB_GAME_TEMPLATE_INPUT_H
