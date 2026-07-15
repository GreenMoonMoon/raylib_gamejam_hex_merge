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

typedef enum PlayMode {
    PLAYMODE_NONE,
    PLAYMODE_DEFAULT,
    PLAYMODE_BUILD,
} PlayMode;

typedef struct Inputs {
    Vector2 moveVector;
    HexCoord selectedCell;
    PlayMode changeMode;
    bool interacts;
} Inputs;

void ProcessInputs(Inputs *inputs);

#endif //RAYLIB_GAME_TEMPLATE_INPUT_H
