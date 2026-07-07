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

#define ANIM_SPEED 24

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Camera3D camera;

static Model player_model;
static ModelAnimation *player_animations;
static int player_anim_count;
static int player_current_anim;
static float player_anim_frame;
static HexCoord player_coordinate;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static void draw_debug_info(const int x, const int y) {
    DrawText(TextFormat("Player anim frame: %.2f", player_anim_frame), x, y, 20, DARKGRAY);
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

    Texture colormap = LoadTexture("resources/textures/colormap.png");

    player_model = LoadModel("resources/models/character_female_a.glb");
    player_model.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
    player_animations = LoadModelAnimations("resources/models/character_female_a.glb", &player_anim_count);
    player_current_anim = 1;
    player_anim_frame = 0;
    player_coordinate = (HexCoord){1, 0, -1}; // hex coordinate component sum is always 0

    // initialize scene

}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    const float frame_time = GetFrameTime();

    // update player animation
    player_anim_frame = fmodf(
        player_anim_frame + frame_time * ANIM_SPEED,
        (float)player_animations[player_current_anim].keyframeCount
    );
    UpdateModelAnimation(player_model, player_animations[player_current_anim], player_anim_frame);
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
    const Vector2 player_position = HexCoordToPosition(player_coordinate);
    DrawModel(player_model, (Vector3){player_position.x + 0.25f, 0, player_position.y + 0.25f}, 2.0f, WHITE);

    EndMode3D();

    // DEBUG
    draw_debug_info(10, 30);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadModel(player_model);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}