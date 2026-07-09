#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "hex.h"
#include "input.h"
#include "player.h"
#include "draw_utils.h"

#define CAMERA_SPEED 4.0f

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

// scene
static HexMap map;

// camera
Camera3D camera;
static Vector3 cameraOffset;

// inputs
static Inputs inputs;
// Vector2 moveDirection;
// HexDirection hexMoveDir;
// HexCoord touchedCell;


//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static float LerpAngle(const float a, const float b, const float alpha)
{
#define TAU 6.28318530718
    const float sa = fmodf(fmodf(b - a, TAU) + 3.0f * PI, TAU) - PI;
    return a + sa * alpha;
}

static void DrawDebugInfo(const int x, const int y) {
    int row = 0;
    // // animation frame
    // DrawText(TextFormat("Animation frame: %.2f", playerAnimFrame), x, y + (++row) * 20, 20, DARKGRAY);

    // hex coordinate
    const HexCoord playerCoordinate = player.coordinate;
    DrawText(TextFormat("Coordinate: q:%d  r:%d", playerCoordinate.q, playerCoordinate.r), x,
             y + (++row) * 20, 20, DARKGRAY);

    // player state
    const char *state_name[] = {"IDLE", "MOVING"};
    DrawText(TextFormat("State: %s", state_name[player.state]), x, y + (++row) * 20, 20, DARKGRAY);

    // player rotation
    DrawText(TextFormat("Rotation: %d", (int)(player.rotation * RAD2DEG)), x, y + (++row) * 20, 20, DARKGRAY);

    // hex direction
    DrawText(TextFormat("Hex direction: %d", inputs.hexMoveDir), x, y + (++row) * 20, 20, DARKGRAY);
}

void DrawDebugInputs(void)
{
    const Vector2 playerPosition = player.position;
    const Vector3 pos = {playerPosition.x + GRID_OFFSET_X, 2.0f, playerPosition.y + GRID_OFFSET_Y};
    DrawCircle3D(pos, 1.5f, (Vector3){1.0f, 0, 0}, 90, DARKBLUE);
    DrawLine3D( pos, (Vector3){pos.x + inputs.moveVector.x * 1.5f, pos.y, pos.z + inputs.moveVector.y * 1.5f}, RED );
    const float angle = (float)(inputs.hexMoveDir + 3) * 1.0471975512f;
    DrawLine3D( pos, Vector3Add(pos, (Vector3){sinf(-angle), 0, cosf(angle)}), BLUE );
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    camera = (Camera3D) {
        .position = (Vector3){0, 8.0f, 8.0f},
        .target = (Vector3){0},
        .up = (Vector3){0, 1.0f, 0},
        .fovy = 70.0f,


        .projection = CAMERA_PERSPECTIVE
    };
    cameraOffset = (Vector3){0, 8.0f, 8.0f};

    // initialize player
    LoadPlayer();

    // initialize scene
    map = (HexMap){10, 10};
    map.cells = calloc(10 * 10, sizeof(int));

    // create obstacles
    for (int i = 0; i < 6; ++i) {
        const int q = GetRandomValue(0, 9);
        const int r = GetRandomValue(0, 9);
        map.cells[q * map.sizeQ + r] = 1;
    }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    const float frameTime = GetFrameTime();

    ProcessInputs(&inputs);

    if (inputs.state != IS_NONE) { MovePlayer(inputs.hexMoveDir); }
    UpdatePlayer(frameTime);

    // update camera
    const Vector2 playerPosition = player.position;
    camera.target = Vector3Lerp(camera.target, (Vector3){playerPosition.x, 0, playerPosition.y}, CAMERA_SPEED * frameTime);
    camera.position = Vector3Add(camera.target, cameraOffset);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    BeginMode3D(camera);

    // DrawHexGrid(20, 10);
    DrawHexMapGrid(map);
    DrawHexWire(player.coordinate, 0.1f, BLUE); // draw actual player coordinate

    // DEBUG
    DrawHex(inputs.touchedCell, -0.2f, RED);
    DrawDebugInputs();

    // draw player
    // DrawModel(playerModel, (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y} , 2.0f, WHITE);
    DrawModelEx(
        player.model,
        (Vector3){player.position.x + GRID_OFFSET_X, 0, player.position.y + GRID_OFFSET_Y},
        (Vector3){0, 1.0f, 0},
        player.rotation * RAD2DEG + 90,
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
    free(map.cells);
    UnloadPlayer();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}