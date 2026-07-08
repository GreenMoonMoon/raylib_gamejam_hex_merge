//
// Created by green on 2026-07-08.
//

#include "player.h"
#include "raymath.h"
#include "input.h"

#define DEFAULT_ANIM_SPEED 24
#define STEP_SOUND_DELAY 0.35f

Model playerModel;
ModelAnimation *playerAnimations;
int playerAnimCount;
int playerCurrentAnim;
float playerAnimFrame;
float playerAnimSpeed;
HexCoord playerCoordinate;
Vector2 playerPosition;
float playerRotation;
PlayerState playerState;

static Sound stepSound = { 0 };
static float stepSoundTimer = 0;

extern HexDirection hexMoveDir;

void LoadPlayer(void)
{
    Texture colormap = LoadTexture("resources/textures/colormap.png");

    playerModel = LoadModel("resources/models/character_female_b.glb");
    playerModel.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
    playerAnimations = LoadModelAnimations("resources/models/character_female_a.glb", &playerAnimCount);
    playerCurrentAnim = 1;
    playerAnimFrame = 0;
    playerAnimSpeed = DEFAULT_ANIM_SPEED;
    playerCoordinate = (HexCoord){0, 0}; // hex coordinate component sum is always 0
    playerPosition = HexCoordToPosition(playerCoordinate);
    playerRotation = PI;

    // SFX
    stepSound = LoadSound("resources/sfx/step.wav");
}

void UnloadPlayer(void)
{
    UnloadSound(stepSound);

    UnloadModelAnimations(playerAnimations, playerAnimCount);
    UnloadModel(playerModel);
}

void UpdatePlayer(const float frameTime)
{
#define MOVE_TIME 0.25f
    static Vector2 lastPlayerPosition;
    static Vector2 nextPlayerPosition;
    // static int angle;
    static float moveFrame;
    float angle = playerRotation;

    switch (playerState) {
        case PS_MOVING:
            stepSoundTimer -= frameTime;
            if (stepSoundTimer <= 0) {
                PlaySound(stepSound);
                stepSoundTimer = STEP_SOUND_DELAY;
            }
            if (moveFrame < MOVE_TIME) {
                moveFrame += frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);
            } else if (isMoving) {
                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                playerCoordinate = HexCoordSubtract(playerCoordinate, hexDirections[hexMoveDir]);

                nextPlayerPosition = HexCoordToPosition(playerCoordinate);
                moveFrame = frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);

                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            } else {
                // switch to idle
                playerState = PS_IDLE;
                playerCurrentAnim = 1;
                playerAnimSpeed = DEFAULT_ANIM_SPEED;
                // playerAnimFrame = 0; // no need to reset the idle animation
                moveFrame = 0;
            }
            break;
        case PS_IDLE:
            if (isMoving) {
                // switch to moving
                playerState = PS_MOVING;
                playerCurrentAnim = 3;
                playerAnimSpeed = 48; // double speed
                playerAnimFrame = 0;

                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                playerCoordinate = HexCoordSubtract(playerCoordinate, hexDirections[hexMoveDir]);

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
}