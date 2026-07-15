#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "hex.h"
#include "map.h"
#include "input.h"
#include "player.h"
#include "draw_utils.h"

#define CAMERA_SPEED 4.0f

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static PlayMode playMode = PLAYMODE_DEFAULT;

// scene
static HexMap map;
static HexCoord selectedCell;

// camera
Camera3D camera;
static Vector3 cameraOffset;

// inputs
static Inputs inputs;
static bool show_build_menu = false;
static int build_menu_cursor = 0;

// player
Player player;

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
    // play mode
    const char *playModeStr[] = {
        "INVALID",
        "Default",
        "Build"
    };
    DrawText(TextFormat("MODE: %s", playModeStr[playMode]), x, y + (++row) * 20, 20, DARKGRAY);

    // // animation frame
    // DrawText(TextFormat("Animation frame: %.2f", playerAnimFrame), x, y + (++row) * 20, 20, DARKGRAY);

    // hex coordinate
    // const HexCoord playerCoordinate = player.coordinate;
    // DrawText(TextFormat("Coordinate: q:%d  r:%d", playerCoordinate.q, playerCoordinate.r), x, y + (++row) * 20, 20, DARKGRAY);

    // player state
    // const char *state_name[] = {"IDLE", "MOVING"};
    // DrawText(TextFormat("State: %s", state_name[player.state]), x, y + (++row) * 20, 20, DARKGRAY);

    // player rotation
    // DrawText(TextFormat("Rotation: %d", (int)(player.rotation * RAD2DEG)), x, y + (++row) * 20, 20, DARKGRAY);

    // hex direction
    // DrawText(TextFormat("Hex direction: %d", inputs.hexMoveDir), x, y + (++row) * 20, 20, DARKGRAY);
}

void DrawDebugInputs(void)
{
    const Vector2 playerPosition = player.position;
    const Vector3 gizmoPosition = {playerPosition.x , 2.0f, playerPosition.y};

    DrawCircle3D(gizmoPosition, 1.5f, (Vector3){1.0f, 0, 0}, 90, DARKBLUE);
    DrawLine3D( gizmoPosition, (Vector3){gizmoPosition.x + inputs.move_vector.x * 1.5f, gizmoPosition.y, gizmoPosition.z + inputs.move_vector.y * 1.5f}, RED );
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
    LoadPlayerResources();
    player = CreatePlayer();

    // initialize scene
    map = generate_map();
    currentMap = &map;
}

void UpdateGameplayScreen(void) {
    const float frame_time = GetFrameTime();

    // reset transient values
    inputs.interacts = false;
    inputs.close = false;
    inputs.toggle_build = false;
    ProcessInputs(&inputs);

    if (inputs.toggle_build) {
        show_build_menu = !show_build_menu;
    }

    if (show_build_menu) {
        if (inputs.close) {
            show_build_menu = false;
        }
    } else {
        if (inputs.interacts) {
            const HexCell *cell = GetMapCell(currentMap, HexCoordAdd(player.coordinate, hexDirections[player.target_direction]));
            if ((cell->type & CELLTYPE_CAN_INTERACT) != 0) {
                if ((cell->type & CELLTYPE_CAN_BUILD) != 0) {
                    show_build_menu = true;
                }
            }
        }
    }

    // update player
    MovePlayer(&player, inputs.move_vector, frame_time);
    selectedCell = inputs.selected_cell;
    UpdatePlayer(&player, frame_time);

    // update camera
    const Vector2 playerPosition = player.position;
    camera.target = Vector3Lerp(camera.target, (Vector3){playerPosition.x, 0, playerPosition.y}, CAMERA_SPEED * frame_time);
    camera.position = Vector3Add(camera.target, cameraOffset);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
    BeginMode3D(camera);

    // DrawHexGrid(20, 10);
    DrawHexMapGrid(map);
    DrawHexWire(player.coordinate, 0.1f, BLUE); // draw actual player coordinate

    // TODO: build a display setup for quick rendering?
    for (int i = 0; i < map.sizeQ * map.sizeR; ++i) {
        if ((map.layers[0][i].type & (CELLTYPE_CAN_INTERACT | CELLTYPE_CAN_BUILD)) != 0) {
            const Vector2 position = HexCoordToPosition(currentMap->layers[0][i].coord);
            DrawCube((Vector3){position.x, 0, position.y}, 0.5f, 0.5f, 0.5f, RED);
        }
    }

    // DEBUG
    DrawHex(HexCoordAdd(player.coordinate, hexDirections[player.target_direction]), -0.1f, GREEN);
    DrawHex(selectedCell, -0.2f, ORANGE);
    DrawDebugInputs();

    // draw player
    // DrawModel(playerModel, (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y} , 2.0f, WHITE);
    DrawModelEx(
        player.model,
        (Vector3){player.position.x, 0, player.position.y},
        (Vector3){0, 1.0f, 0},
        player.rotation * RAD2DEG + 90,
        (Vector3){2.0f, 2.0f, 2.0f},
        WHITE
    );

    EndMode3D();

    if (show_build_menu) {
        DrawRectangleGradientV(20, 20, 200, 400, GRAY, DARKGRAY);
        DrawRectangleGradientV(30, 30, 85, 85, DARKBLUE, BLUE);

        // draw cursor
        DrawRectangleLines(30, 30, 85, 85, ORANGE);
    }

    // DEBUG
    DrawDebugInfo(10, 30);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // DeleteHexMap(map);
    UnloadPlayerResources();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}