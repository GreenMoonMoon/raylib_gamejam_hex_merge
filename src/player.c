//
// Created by green on 2026-07-08.
//

#include "player.h"
#include "raymath.h"

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

static void stopPlayer(Player *player) {
    player->state = PS_IDLE;
    player->currentAnimation = 1;
    player->animationSpeed = DEFAULT_ANIM_SPEED;
    // playerAnimFrame = 0; // no need to reset the idle animation
    moveFrame = 0;
}

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

    // UnloadModelAnimations(playerAnimations, playerAnimationCount);
    // UnloadModel(playerModel);
}

Player CreatePlayer(void)
{
    const Player player = {
        .state = PS_IDLE,
        .model = playerModel,
        .animations = playerAnimations,
        .currentAnimation = 1,
        .animationFrame = 0,
        .animationSpeed = DEFAULT_ANIM_SPEED,
        .coordinate = (HexCoord){0, 0},
        .targetCoordinate = (HexCoord){0, 0},
        .position = HexCoordToPosition((HexCoord){0, 0}),
        .rotation = PI
    };
    return player;
}

void UpdatePlayer(Player *player, const Inputs inputs, const float frameTime)
{
    switch (player->state) {
        case PS_MOVING:
            // play step sound
            stepSoundTimer -= frameTime;
            if (stepSoundTimer <= 0) {
                PlaySound(stepSound);
                stepSoundTimer = STEP_SOUND_DELAY;
            }

            if (moveFrame < MOVE_TIME) { // if the movement isn't finished, update position
                moveFrame += frameTime;
                player->position = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);
            } else {
                moveFrame = 0; // reset movement

                // if movement is finished check if the inputs ask for a movement
                if (inputs.state == IS_TOUCH_DRAG || inputs.state == IS_KEYBOARD_DPAD) {
                    const HexCoord nextCoordinate = GetMapNeighbor(player->coordinate, inputs.hexMoveDir);
                    if (!CheckMapCollision(nextCoordinate)) {
                        lastPlayerPosition = HexCoordToPosition(player->coordinate);
                        player->coordinate = nextCoordinate;
                        nextPlayerPosition = HexCoordToPosition(player->coordinate);

                        player->rotation = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
                    } else { stopPlayer(player); }
                } else if (inputs.state == IS_TOUCH_SELECT) {
                    // if (!HexCoordEqual(player->coordinate, player->targetCoordinate)) {
                    //     const HexCoord nextCoordinate = PathNextMapCoordinate(player->coordinate, player->targetCoordinate);
                    //
                    //     lastPlayerPosition = HexCoordToPosition(player->coordinate);
                    //     player->coordinate = nextCoordinate;
                    //     nextPlayerPosition = HexCoordToPosition(player->coordinate);
                    //
                    //     player->rotation = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
                    // } else {
                    //     stopPlayer(player);
                    // }
                } else { stopPlayer(player); }
            }
            break;
        case PS_IDLE:
            if (inputs.state == IS_TOUCH_DRAG || inputs.state == IS_KEYBOARD_DPAD) {
                const HexCoord nextCoordinate = GetMapNeighbor(player->coordinate, inputs.hexMoveDir);
                if (!CheckMapCollision(nextCoordinate)) {
                    player->state = PS_MOVING;

                    // set the running animation
                    player->currentAnimation = 3;
                    player->animationSpeed = 48; // double speed
                    player->animationFrame = 0;

                    lastPlayerPosition = HexCoordToPosition(player->coordinate);
                    player->coordinate = nextCoordinate;
                    nextPlayerPosition = HexCoordToPosition(player->coordinate);

                    player->rotation = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
                }
            } else if (inputs.state == IS_TOUCH_SELECT) {
                // player->state = PS_MOVING;
                //
                // // set the running animation
                // player->currentAnimation = 3;
                // player->animationSpeed = 48; // double speed
                // player->animationFrame = 0;
                //
                // player->targetCoordinate = inputs.touchedCell;
                // const HexCoord nextCoordinate = PathNextMapCoordinate(player->coordinate, player->targetCoordinate);
                //
                // lastPlayerPosition = HexCoordToPosition(player->coordinate);
                // player->coordinate = nextCoordinate;
                // nextPlayerPosition = HexCoordToPosition(player->coordinate);
                //
                // player->rotation = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            }
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