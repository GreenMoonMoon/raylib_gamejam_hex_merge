//
// Created by green on 2026-07-08.
//

#include "input.h"
#include "raylib.h"
#include "raymath.h"

#define M_PI_3 1.0471975512f
#define MOVE_INPUT_CUTOFF 0.1f

extern Camera camera;

static bool gameIsPaused = false;
static InputState state = IS_NONE;

static int lastGesture;
static Vector2 lastKeyMoveInput;

static Vector2 GetScreenToGround(const Vector2 screenPosition) {
    const Ray ray = GetScreenToWorldRay(screenPosition, camera);
    const float d = (-ray.position.y / ray.direction.y);
    const Vector3 worldPosition = Vector3Add(ray.position, Vector3Scale(ray.direction, d));
    return (Vector2){worldPosition.x, worldPosition.z};
}

static void ProcessTouchInputs(Inputs *inputs, const HCAxial playerCoordinate) {
    // GESTURES
    const int currentGesture = GetGestureDetected();
    const Vector2 touchPosition = GetTouchPosition(0);

    if (currentGesture != GESTURE_NONE) { // process gesture
        // get world touch position
        const Vector2 touchWorldPosition = GetScreenToGround(touchPosition);

        // if it is a new touch event, check if the player is trying to drag the player character or select a destination
        if (lastGesture == GESTURE_NONE) {
            // get the cell coordinate from the touch position
            inputs->selected_cell = PositionToHCA((Vector2){touchWorldPosition.x, touchWorldPosition.y});
            if (HCAEqual(inputs->selected_cell, playerCoordinate)) {
                state = IS_TOUCH_DRAG;
            } else {
                state = IS_TOUCH_SELECT;
            };
        }

        if (state == IS_TOUCH_DRAG) {
            const Vector2 playerPosition = HCAToPosition(playerCoordinate);
            const Vector2 dragVector = Vector2Subtract((Vector2){touchWorldPosition.x - GRID_OFFSET_X, touchWorldPosition.y - GRID_OFFSET_Y}, playerPosition);
            const float dragVectorLength = Vector2Length(dragVector);
            if (dragVectorLength > 1.0f) { inputs->move_vector = Vector2Scale(dragVector, 1.0f / dragVectorLength); }
        }
    } else {
        if (state == IS_TOUCH_DRAG) {
            state = IS_NONE;
            inputs->move_vector = (Vector2){0};
        } // stop drag inputs
    }
    lastGesture = currentGesture;
}

static void ProcessKeyboardInputs(Inputs *inputs) {
    // // TODO: buffer 2 or 3 frame inputs, then calculate the angle and divide by 6 directions to use in a switch statement
    Vector2 keyMoveInput = {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };

    if (Vector2LengthSqr(keyMoveInput) > MOVE_INPUT_CUTOFF) {
        keyMoveInput = Vector2Normalize(keyMoveInput);
        inputs->move_vector = Vector2Lerp(inputs->move_vector, keyMoveInput, 0.25f);
    } else {
        inputs->move_vector = (Vector2){0};
    }

    // target a cell
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        const Vector2 clickWorldPosition = GetScreenToGround(GetMousePosition());
        inputs->selected_cell = PositionToHCA((Vector2){clickWorldPosition.x, clickWorldPosition.y});
    }

    lastKeyMoveInput = keyMoveInput;

    if (IsKeyPressed(KEY_E)) {
        inputs->interacts = true;
    }

    if (IsKeyPressed(KEY_B)) {
        // TODO: enter build mode
        inputs->toggle_build = true;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        inputs->close = true;
    }

    if (IsKeyPressed(KEY_PAUSE)) {
        gameIsPaused = !gameIsPaused;
        TraceLog(LOG_DEBUG, "PAUSE KEY PRESSED");
    }
}

static void ProcessGamepadInputs(Inputs *inputs) {
}

void ProcessInputs(Inputs *inputs) {
    ProcessKeyboardInputs(inputs);
}
