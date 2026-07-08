//
// Created by green on 2026-07-08.
//

#ifndef RAYLIB_GAME_TEMPLATE_INPUT_H
#define RAYLIB_GAME_TEMPLATE_INPUT_H

extern bool isMoving;

typedef enum InputState {
    IS_NONE,
    IS_TOUCH_DRAG,
    IS_TOUCH_SELECT,
    IS_KEYBOARD_DPAD,
} InputState;

void ProcessInputs(void);

#endif //RAYLIB_GAME_TEMPLATE_INPUT_H
