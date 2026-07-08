//
// Created by green on 2026-07-08.
//

#include "input.h"
#include "raylib.h"
#include "raymath.h"
#include "hex.h"

#define M_PI_3 1.0471975512f

extern Camera camera;

static InputState inputState;
static int lastGesture;
static Vector3 touchedPosition;

// TODO: decouple from input module
extern HexCoord playerCoordinate;
extern Vector2 playerPosition;
extern Vector2 moveDirection;
extern HexCoord touchedCell;
extern HexDirection hexMoveDir;

bool isMoving;

// TODO: split function to uses various parts in the update function directly
void ProcessInputs(void) {
    // handle touch input
    const int currentGesture = GetGestureDetected();
    const Vector2 touchPosition = GetTouchPosition(0);

    if (currentGesture != GESTURE_NONE) {
        const Ray ray = GetScreenToWorldRay(touchPosition, camera);
        const float d = (-ray.position.y / ray.direction.y);
        touchedPosition = Vector3Add(ray.position, Vector3Scale(ray.direction, d));

        if (lastGesture == GESTURE_NONE) {
            // get the cell coordinate from the touch position
            touchedCell = PositionToHexCoord((Vector2){touchedPosition.x - GRID_OFFSET_X, touchedPosition.z - GRID_OFFSET_Y});
            inputState = HexCoordEqual(touchedCell, playerCoordinate) ? IS_TOUCH_DRAG : IS_TOUCH_SELECT;
        }

        switch (inputState) {
            case  IS_TOUCH_DRAG:
                const Vector2 v = Vector2Subtract((Vector2){touchedPosition.x - GRID_OFFSET_X, touchedPosition.z - GRID_OFFSET_Y}, playerPosition);
                const float vlen = Vector2Length(v);
                if (vlen > 1.0f) { moveDirection = Vector2Scale(v, 1.0f / vlen); }
                 break;
            case IS_TOUCH_SELECT:
                // TODO: implement move command and pathing
                break;
            default: break;
        }
    } else {
        if (inputState == IS_TOUCH_DRAG) {
            moveDirection = (Vector2){0};
            inputState = IS_NONE;
        }
    }
    lastGesture = currentGesture;

    // handle keyboard input
    Vector2 moveInput = {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };
    if (fabsf(moveInput.x) > EPSILON || fabsf(moveInput.y) > EPSILON) {
        inputState = IS_KEYBOARD_DPAD;
        moveInput = Vector2Normalize(moveInput);
    }

    if (inputState == IS_KEYBOARD_DPAD) {
        // FIXME: replace with a radial lerp?
        moveDirection = Vector2Lerp(moveDirection, moveInput, 0.25f);
    }

    if (IsKeyPressed(KEY_PAUSE)) {
        TraceLog(LOG_DEBUG, "PAUSE KEY PRESSED");
    }

    // // TODO: buffer 2 or 3 frame inputs, then calculate the angle and divide by 6 directions to use in a switch statement
#define IS_MOVING_CUTOFF 0.1f
    isMoving = Vector2LengthSqr(moveDirection) > IS_MOVING_CUTOFF;
    if (isMoving) {
        // get the hex direction
        hexMoveDir = (int)roundf(atan2f(-moveDirection.x, moveDirection.y) / M_PI_3) + 3;
        hexMoveDir = hexMoveDir % 6;
    }
}
