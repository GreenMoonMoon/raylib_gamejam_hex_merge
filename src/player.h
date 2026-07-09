//
// Created by green on 2026-07-08.
//

#ifndef RAYLIB_GAME_TEMPLATE_PLAYER_H
#define RAYLIB_GAME_TEMPLATE_PLAYER_H

#include "raylib.h"
#include "hex.h"
#include "input.h"

typedef enum PlayerState {
    PS_IDLE,
    PS_MOVING,
} PlayerState;

typedef struct Player {
    PlayerState state;
    Model model;
    ModelAnimation *animations;
    int currentAnimation;
    float animationFrame;
    float animationSpeed;
    HexCoord coordinate;
    HexCoord nextCoordinate;
    Vector2 position;
    float rotation;
} Player;

void LoadPlayerResources(void);

void UnloadPlayerResources(void);

Player CreatePlayer(void);

void UpdatePlayer(Player *player, Inputs inputs, float frameTime);

#endif //RAYLIB_GAME_TEMPLATE_PLAYER_H
