//
// Created by green on 2026-07-08.
//

#include "player.h"
#include "raymath.h"
#include "input.h"

#define MOVE_TIME 0.25f
#define DEFAULT_ANIM_SPEED 24
#define STEP_SOUND_DELAY 0.35f

// Model
static Model playerModel;
static ModelAnimation *playerAnimations;
static int playerAnimationCount;

// SFX
static Sound stepSound = { 0 };
static float stepSoundTimer = 0;

// used for move interpolation
static Vector2 lastPlayerPosition;
static Vector2 nextPlayerPosition;
static float moveFrame;

void LoadPlayerResources(void)
{
    // Model
    const Texture colormap = LoadTexture("resources/textures/colormap.png");
    playerModel = LoadModel("resources/models/character_female_b.glb");
    playerModel.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;

    playerAnimations = LoadModelAnimations("resources/models/character_female_b.glb", &playerAnimationCount);

    // SFX
    stepSound = LoadSound("resources/sfx/step.wav");
}

void UnloadPlayerResources(void)
{
    UnloadSound(stepSound);

    UnloadModelAnimations(playerAnimations, playerAnimationCount);
    UnloadModel(playerModel);
}

Player CreatePlayer(void)
{
    Player player = {
        .state = PS_IDLE,
        .model = playerModel,
        .animations = playerAnimations,
        .currentAnimation = 1,
        .animationFrame = 0,
        .animationSpeed = DEFAULT_ANIM_SPEED,
        .coordinate = (HexCoord){0, 0},
        .nextCoordinate = (HexCoord){0, 0},
        .position = HexCoordToPosition((HexCoord){0, 0}),
        .rotation = PI
    };
    return player;
}

void MovePlayer(Player *player, const HexDirection direction)
{
    if (player->state != PS_MOVING) {
        player->nextCoordinate = HexCoordSubtract(player->coordinate, hexDirections[direction]);
        player->state = PS_MOVING;

        // set the running animation
        player->currentAnimation = 3;
        player->animationSpeed = 48; // double speed
        // player->animationFrame = 0;

        lastPlayerPosition = HexCoordToPosition(player->coordinate);
        nextPlayerPosition = HexCoordToPosition(player->nextCoordinate);

        player->coordinate = player->nextCoordinate; // update coordinate
        player->rotation = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
    }
}

void UpdatePlayer(Player *player, const float frameTime)
{
    switch (player->state) {
        case PS_MOVING:
            // play step sound
            stepSoundTimer -= frameTime;
            if (stepSoundTimer <= 0) {
                PlaySound(stepSound);
                stepSoundTimer = STEP_SOUND_DELAY;
            }

            // update move interpolation
            if (moveFrame < MOVE_TIME) {
                moveFrame += frameTime;
                player->position = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);
            } else {
                // switch to idle if the movement is finished
                player->state = PS_IDLE;
                player->currentAnimation = 1;
                player->animationSpeed = DEFAULT_ANIM_SPEED;
                // playerAnimFrame = 0; // no need to reset the idle animation
                moveFrame = 0;
            }
            break;
        case PS_IDLE:
        default:
            break;
    }

    // update player model
    // -- update animation
    player->animationFrame = fmodf(
        player->animationFrame + frameTime * player->animationSpeed,
        (float)player->animations[player->currentAnimation].keyframeCount
    );
    UpdateModelAnimation(player->model, player->animations[player->currentAnimation], player->animationFrame);
}