//
// Created by green on 2026-07-08.
//

#include "input.h"
#include "raylib.h"
#include "raymath.h"

#define M_PI_3 1.0471975512f
#define MOVE_INPUT_CUTOFF 0.1f

extern Camera camera;

static int lastGesture;
static Vector3 touchWorldPosition;
static Vector2 lastKeyMoveInput;

void ProcessInputs(Inputs *inputs, HexCoord playerCoordinate) {
    // GESTURES
    const int currentGesture = GetGestureDetected();
    const Vector2 touchPosition = GetTouchPosition(0);

    if (currentGesture != GESTURE_NONE) { // process gesture
        // get world touch position
        const Ray ray = GetScreenToWorldRay(touchPosition, camera);
        const float d = (-ray.position.y / ray.direction.y);
        touchWorldPosition = Vector3Add(ray.position, Vector3Scale(ray.direction, d));

        // if it is a new touch event, check if the player is trying to drag the player character or select a destination
        if (lastGesture == GESTURE_NONE) {
            // get the cell coordinate from the touch position
            inputs->touchedCell = PositionToHexCoord((Vector2){touchWorldPosition.x - GRID_OFFSET_X, touchWorldPosition.z - GRID_OFFSET_Y});
            if (HexCoordEqual(inputs->touchedCell, playerCoordinate)) {
                inputs->state = IS_TOUCH_DRAG;
            } else {
                inputs->state = IS_TOUCH_SELECT;
            };
        }

        if (inputs->state == IS_TOUCH_DRAG) {
            const Vector2 playerPosition = HexCoordToPosition(playerCoordinate);
            const Vector2 dragVector = Vector2Subtract((Vector2){touchWorldPosition.x - GRID_OFFSET_X, touchWorldPosition.z - GRID_OFFSET_Y}, playerPosition);
            const float dragVectorLength = Vector2Length(dragVector);
            if (dragVectorLength > 1.0f) { inputs->moveVector = Vector2Scale(dragVector, 1.0f / dragVectorLength); }
        }
    } else {
        if (inputs->state == IS_TOUCH_DRAG) {
            inputs->state = IS_NONE;
            inputs->moveVector = (Vector2){0};
        } // stop drag inputs
    }
    lastGesture = currentGesture;

    // KEYBOARD
    // // TODO: buffer 2 or 3 frame inputs, then calculate the angle and divide by 6 directions to use in a switch statement
    Vector2 keyMoveInput = {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };

    if (Vector2LengthSqr(keyMoveInput) > MOVE_INPUT_CUTOFF) {
        inputs->state = IS_KEYBOARD_DPAD;

        keyMoveInput = Vector2Normalize(keyMoveInput);
        inputs->moveVector = Vector2Lerp(inputs->moveVector, keyMoveInput, 0.25f);
    } else if (inputs->state == IS_KEYBOARD_DPAD) {
        inputs->state = IS_NONE;
        inputs->moveVector = (Vector2){0};
    }

    switch (inputs->state) {
        case IS_TOUCH_DRAG:
        case IS_KEYBOARD_DPAD:
            inputs->hexMoveDir = (int)roundf(atan2f(-inputs->moveVector.x, inputs->moveVector.y) / M_PI_3) + 3;
            inputs->hexMoveDir = inputs->hexMoveDir % 6;
            break;
        case IS_TOUCH_SELECT:
            // TODO: handle select ...
            inputs->state = IS_NONE;
        case IS_NONE:
            break;
    }
    lastKeyMoveInput = keyMoveInput;

    if (IsKeyPressed(KEY_PAUSE)) {
        TraceLog(LOG_DEBUG, "PAUSE KEY PRESSED");
    }
}
