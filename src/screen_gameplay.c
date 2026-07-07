/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "hex.h"
#include "draw_utils.h"

#define DEFAULT_ANIM_SPEED 24
#define CAMERA_SPEED 4.0f

#define STEP_SOUND_DELAY 0.35f

// TODO: move to player module
typedef enum PlayerState {
    PS_IDLE,
    PS_MOVING,
} PlayerState;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Camera3D camera;
static Vector3 camera_offset;

static Model playerModel;
static ModelAnimation *playerAnimations;
static int playerAnimCount;
static int playerCurrentAnim;
static float playerAnimFrame;
static float playerAnimSpeed;
static PlayerState playerState;
static HexCoord playerCoordinate;
static Vector2 playerPosition;
static float playerRotation;

static Sound step_sound = { 0 };
static float step_sound_timer = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static void DrawDebugInfo(const int x, const int y) {
    // animation frame
    DrawText(TextFormat("Animation frame: %.2f", playerAnimFrame), x, y, 20, DARKGRAY);

    // hex coordinate
    DrawText(TextFormat("Coordinate: q:%d  r:%d", playerCoordinate.q, playerCoordinate.r), x,
             y + 20, 20, DARKGRAY);

    // player state
    const char *state_name[] = {"IDLE", "MOVING"};
    DrawText(TextFormat("State: %s", state_name[playerState]), x, y + 40, 20, DARKGRAY);

    // player rotation
    DrawText(TextFormat("Rotation: %d", (int)(playerRotation * RAD2DEG)), x, y + 60, 20, DARKGRAY);
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    camera = (Camera3D) {
        .position = (Vector3){0, 8.0f, 8.0f},
        .target = (Vector3){0},
        .up = (Vector3){0, 1.0f, 0},
        .fovy = 70.0f,


        .projection = CAMERA_PERSPECTIVE
    };
    camera_offset = (Vector3){0, 8.0f, 8.0f};

    Texture colormap = LoadTexture("resources/textures/colormap.png");

    playerModel = LoadModel("resources/models/character_female_a.glb");
    playerModel.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
    playerAnimations = LoadModelAnimations("resources/models/character_female_a.glb", &playerAnimCount);
    playerCurrentAnim = 1;
    playerAnimFrame = 0;
    playerAnimSpeed = DEFAULT_ANIM_SPEED;
    playerCoordinate = (HexCoord){0, 0}; // hex coordinate component sum is always 0
    playerPosition = HexCoordToPosition(playerCoordinate);
    playerRotation = PI;

    // SFX
    step_sound = LoadSound("resources/sfx/step.wav");

    // initialize scene

}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    const float frameTime = GetFrameTime();

    // TODO: buffer 2 or 3 frame inputs, then calculate the angle and divide by 6 directions to use in a switch statement
    // get inputs
    const bool move_up = IsKeyDown(KEY_W);
    const bool move_down = IsKeyDown(KEY_S);
    const bool move_left = IsKeyDown(KEY_A);
    const bool move_right = IsKeyDown(KEY_D);
    const char any_inputs = move_up | move_down << 1 | move_left << 2 | move_right << 3;

    // manage player state machine
#define MOVE_TIME 0.25f
    static Vector2 lastPlayerPosition;
    static Vector2 nextPlayerPosition;
    // static int angle;
    static float move_frame;

    float angle = playerRotation;
    switch (playerState) {
        case PS_MOVING:
            step_sound_timer -= frameTime;
            if (step_sound_timer <= 0) {
                PlaySound(step_sound);
                step_sound_timer = STEP_SOUND_DELAY;
            }
            if (move_frame < MOVE_TIME) {
                move_frame += frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, move_frame / MOVE_TIME);
            } else if (any_inputs != 0) {
                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                if (move_down && !move_right) { playerCoordinate.q++; }
                if (move_up && !move_left) { playerCoordinate.q--; }
                if (move_right) { playerCoordinate.r++; }
                if (move_left) { playerCoordinate.r--; }

                nextPlayerPosition = HexCoordToPosition(playerCoordinate);
                move_frame = frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, move_frame / MOVE_TIME);

                // player angle
                // angle = (int)floorf(Vector2Angle(lastPlayerPosition, nextPlayerPosition) / 6);
                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            } else {
                // switch to idle
                playerState = PS_IDLE;
                playerCurrentAnim = 1;
                playerAnimSpeed = DEFAULT_ANIM_SPEED;
                // playerAnimFrame = 0; // no need to reset the idle animation
                move_frame = 0;
            }
            break;
        case PS_IDLE:
            if (any_inputs != 0) {
                // switch to moving
                playerState = PS_MOVING;
                playerCurrentAnim = 3;
                playerAnimSpeed = 48; // double speed
                playerAnimFrame = 0;

                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                if (move_down && !move_right) { playerCoordinate.q++; }
                if (move_up && !move_left) { playerCoordinate.q--; }
                if (move_right) { playerCoordinate.r++; }
                if (move_left) { playerCoordinate.r--; }

                nextPlayerPosition = HexCoordToPosition(playerCoordinate);

                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            }
            break;
    }

    // update player model
    // -- update orientation
    playerRotation = angle;
    // -- update animation
    playerAnimFrame = fmodf(
        playerAnimFrame + frameTime * playerAnimSpeed,
        (float)playerAnimations[playerCurrentAnim].keyframeCount
    );
    UpdateModelAnimation(playerModel, playerAnimations[playerCurrentAnim], playerAnimFrame);


    // update camera
    camera.target = Vector3Lerp(camera.target, (Vector3){playerPosition.x, 0, playerPosition.y}, CAMERA_SPEED * frameTime);
    camera.position = Vector3Add(camera.target, camera_offset);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    // Vector2 pos = { 20, 10 };
    // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    // DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

    BeginMode3D(camera);

    DrawHexGrid(20, 10);

    // draw player
    // DrawModel(playerModel, (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y} , 2.0f, WHITE);
    DrawModelEx(
        playerModel,
        (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y},
        (Vector3){0, 1.0f, 0},
        playerRotation * RAD2DEG + 90,
        (Vector3){2.0f, 2.0f, 2.0f},
        WHITE
    );

    EndMode3D();

    // DEBUG
    DrawDebugInfo(10, 30);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadSound(step_sound);

    UnloadModelAnimations(playerAnimations, playerAnimCount);
    UnloadModel(playerModel);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}