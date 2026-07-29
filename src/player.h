//
// Created by green on 2026-07-08.
//

#ifndef RAYLIB_GAME_TEMPLATE_PLAYER_H
#define RAYLIB_GAME_TEMPLATE_PLAYER_H

#include "raylib.h"
#include "hex.h"

typedef enum PlayerState {
    PS_IDLE,
    PS_MOVING,
} PlayerState;

typedef struct Player {
    // state machine
    bool can_move;
    PlayerState state;
    PlayerState next_state;

    // inventory, equipment and slots
    bool hold_resources; // DEBUG

    // model
    Model model;
    ModelAnimation *animations;
    int currentAnimation;
    float animationFrame;
    float animationSpeed;

    // coordinate
    Axial coordinate;
    AxialDirection target_direction;

    Vector2 position;
    float rotation;
} Player;

void LoadPlayerResources(void);

void UnloadPlayerResources(void);

Player CreatePlayer(void);

void UpdatePlayer(Player *player, float frame_time);

void MovePlayer(Player *player, Vector2 movement, float frame_time);

void stop_player(Player *player);

#endif //RAYLIB_GAME_TEMPLATE_PLAYER_H
