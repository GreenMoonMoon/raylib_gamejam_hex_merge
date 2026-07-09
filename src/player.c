//
// Created by green on 2026-07-08.
//

#include "player.h"
#include "raymath.h"
#include "input.h"

#define DEFAULT_ANIM_SPEED 24
#define STEP_SOUND_DELAY 0.35f

Player player;

static Sound stepSound = { 0 };
static float stepSoundTimer = 0;


void LoadPlayer(void)
{
    Texture colormap = LoadTexture("resources/textures/colormap.png");

    player.model = LoadModel("resources/models/character_female_b.glb");
    player.model.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
    player.animations = LoadModelAnimations("resources/models/character_female_a.glb", &player.animationCount);
    player.currentAnimation = 1;
    player.animationFrame = 0;
    player.animationSpeed = DEFAULT_ANIM_SPEED;
    player.coordinate = (HexCoord){0, 0}; // hex coordinate component sum is always 0
    player.position = HexCoordToPosition(player.coordinate);
    player.rotation = PI;

    // SFX
    stepSound = LoadSound("resources/sfx/step.wav");
}

void UnloadPlayer(void)
{
    UnloadSound(stepSound);

    UnloadModelAnimations(player.animations, player.animationCount);
    UnloadModel(player.model);
}

void MovePlayer(const HexDirection direction) {
    player.nextCoordinate = HexCoordSubtract(player.coordinate, hexDirections[direction]);
    if (player.state != PS_MOVING && !HexCoordEqual(player.coordinate, player.nextCoordinate)) {
        player.state = PS_MOVING;
    }
}

void UpdatePlayer(const float frameTime)
{
#define MOVE_TIME 0.25f
    static Vector2 lastPlayerPosition;
    static Vector2 nextPlayerPosition;
    // static int angle;
    static float moveFrame;
    float angle = player.rotation;

    switch (player.state) {
        case PS_MOVING:
            stepSoundTimer -= frameTime;
            if (stepSoundTimer <= 0) {
                PlaySound(stepSound);
                stepSoundTimer = STEP_SOUND_DELAY;
            }
            if (moveFrame < MOVE_TIME) {
                moveFrame += frameTime;
                player.position = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);
            } else if (!HexCoordEqual(player.coordinate, player.nextCoordinate)) {
                lastPlayerPosition = HexCoordToPosition(player.coordinate);

                player.coordinate = player.nextCoordinate;

                nextPlayerPosition = HexCoordToPosition(player.coordinate);
                moveFrame = frameTime;
                player.position = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);

                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            } else {
                // switch to idle
                player.state = PS_IDLE;
                player.currentAnimation = 1;
                player.animationSpeed = DEFAULT_ANIM_SPEED;
                // playerAnimFrame = 0; // no need to reset the idle animation
                moveFrame = 0;
            }
            break;
        case PS_IDLE:
            if (!HexCoordEqual(player.coordinate, player.nextCoordinate)) {
                // switch to moving
                player.state = PS_MOVING;
                player.currentAnimation = 3;
                player.animationSpeed = 48; // double speed
                player.animationFrame = 0;

                lastPlayerPosition = HexCoordToPosition(player.coordinate);

                player.coordinate = player.nextCoordinate;

                nextPlayerPosition = HexCoordToPosition(player.coordinate);

                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            }
            break;
    }

    // update player model
    // -- update orientation
    player.rotation = angle;
    // -- update animation
    player.animationFrame = fmodf(
        player.animationFrame + frameTime * player.animationSpeed,
        (float)player.animations[player.currentAnimation].keyframeCount
    );
    UpdateModelAnimation(player.model, player.animations[player.currentAnimation], player.animationFrame);
}