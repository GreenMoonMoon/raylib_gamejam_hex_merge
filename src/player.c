//
// Created by green on 2026-07-08.
//

#include "player.h"
#include "raymath.h"
#include "map.h"

#define MOVE_TIME 0.25f
#define MOVE_SPEED 5.0f
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
static Vector2 last_player_position;
static Vector2 next_player_position;
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

    UnloadModelAnimations(playerAnimations, playerAnimationCount);
    UnloadModel(playerModel);
}

Player CreatePlayer(void)
{
    const Player player = {
        .state = PS_IDLE,
        .next_state = PS_IDLE,
        .model = playerModel,
        .animations = playerAnimations,
        .currentAnimation = 1,
        .animationFrame = 0,
        .animationSpeed = DEFAULT_ANIM_SPEED,
        .can_move = true,
        .coordinate = (Axial){0, 0},
        .target_direction = HD_NORTH,
        .position = AxialToPosition((Axial){0, 0}),
        .rotation = PI
    };
    return player;
}

void UpdatePlayer(Player *player, const float frame_time) {
    switch (player->state) {
        case PS_MOVING:
            switch (player->next_state) {
                case PS_IDLE:
                    player->state = PS_IDLE;
                    player->currentAnimation = 1;
                    player->animationSpeed = DEFAULT_ANIM_SPEED;
                    break;
                case PS_MOVING:
                    // play step sound
                    stepSoundTimer -= frame_time;
                    if (stepSoundTimer <= 0) {
                        PlaySound(stepSound);
                        stepSoundTimer = STEP_SOUND_DELAY;
                    }
                    break;
            }
            break;
        case PS_IDLE:
            switch (player->next_state) {
                case PS_MOVING:
                    player->state = PS_MOVING;
                    player->animationFrame = 0;
                    player->currentAnimation = 3;
                    player->animationSpeed = DEFAULT_ANIM_SPEED * 2.0f;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    // update player model
    // -- update animation
    player->animationFrame = fmodf(
        player->animationFrame + frame_time * player->animationSpeed,
        (float)player->animations[player->currentAnimation].keyframeCount
    );
    UpdateModelAnimation(player->model, player->animations[player->currentAnimation], player->animationFrame);
}

void MovePlayer(Player *player, const Vector2 movement, const float frame_time) {
    if (Vector2LengthSqr(movement) > 0.1f && player->can_move) {
        player->next_state = PS_MOVING;
        const Vector2 next_position = Vector2Add(player->position, Vector2Scale(movement, frame_time * MOVE_SPEED));
        player->rotation = Vector2LineAngle(next_position, player->position) + PI;
        player->position = next_position;

        player->coordinate = PositionToAxial(player->position);

        player->target_direction = ((int)roundf(atan2f(-movement.x, movement.y) / M_PI_3) + 3) % HD_COUNT;
    } else {
        player->next_state = PS_IDLE;
    }
}

void stop_player(Player *player) {
    player->next_state = PS_IDLE;
}