//
// Created by green on 2026-07-08.
//

#ifndef RAYLIB_GAME_TEMPLATE_PLAYER_H
#define RAYLIB_GAME_TEMPLATE_PLAYER_H

#include "raylib.h"
#include "hex.h"

extern Model playerModel;
extern ModelAnimation *playerAnimations;
extern int playerAnimCount;
extern int playerCurrentAnim;
extern float playerAnimFrame;
extern float playerAnimSpeed;
extern HexCoord playerCoordinate;
extern Vector2 playerPosition;
extern float playerRotation;

typedef enum PlayerState {
    PS_IDLE,
    PS_MOVING,
} PlayerState;
extern PlayerState playerState;

void LoadPlayer(void);

void UnloadPlayer(void);

void UpdatePlayer(float frameTime);

#endif //RAYLIB_GAME_TEMPLATE_PLAYER_H
