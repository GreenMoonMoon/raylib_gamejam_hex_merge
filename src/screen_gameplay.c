// TODO: hex tile specific instance rendering ( save on memory passed to the GPU each frame )
// TODO: shadow map shader
// TODO: ambient occlusion shader

#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "hex.h"
#include "map.h"
#include "input.h"
#include "player.h"
#include "draw_utils.h"
#include "building.h"
#include "pipes.h"
#include "extern/stb_ds.h"

#define CAMERA_SPEED 4.0f

typedef enum PlayMode {
    PLAYMODE_NONE,
    PLAYMODE_DEFAULT,
    PLAYMODE_BUILD,
    PLAYMODE_BUILD_MENU,
} PlayMode;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static PlayMode play_mode = PLAYMODE_DEFAULT;
static BuildingType blueprint = BUILDING_WELL;
static char blueprint_rot = 0;

// scene
static Chunk map;
static Axial selectedCell;

// camera
Camera3D camera;
static Vector3 cameraOffset;

// inputs
static Inputs inputs;
static bool show_build_menu = false;
static int build_menu_cursor = 0;

// player
static Player player;

// TODO: contextualize with a drone?
// Mouse blueprint mode
static bool mouse_pipe_tool = false;
struct BPP {
    Vector2 position;
    char rotation;
    enum PipeModelID id;
};
static struct BPP pipe_tool_start_bpp, pipe_tool_end_bpp;
static struct BPP *pipe_tool_bpp_list; // Blueprint Pipe
static Axial previous_selected_tile;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static float lerp_angle(const float a, const float b, const float alpha) {
#define TAU 6.28318530718
    const float sa = fmodf(fmodf(b - a, TAU) + 3.0f * PI, TAU) - PI;
    return a + sa * alpha;
}

static void ddraw_info(const int x, const int y) {
    int row = 0;
    // // play mode
    // const char *playModeStr[] = {
    //     "INVALID",
    //     "Default",
    //     "Build"
    // };
    // DrawText(TextFormat("MODE: %s", playModeStr[playMode]), x, y + (++row) * 20, 20, DARKGRAY);

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

void ddraw_inputs() {
    const Vector2 playerPosition = player.position;
    const Vector3 gizmoPosition = {playerPosition.x , 2.0f, playerPosition.y};

    DrawCircle3D(gizmoPosition, 1.5f, (Vector3){1.0f, 0, 0}, 90, DARKBLUE);
    DrawLine3D( gizmoPosition, (Vector3){gizmoPosition.x + inputs.move_vector.x * 1.5f, gizmoPosition.y, gizmoPosition.z + inputs.move_vector.y * 1.5f}, RED );
}

void ddraw_chunk_info(const Chunk chunk) {
    const Vector2 wpos = CheckerToPosition(chunk.coord);
    const Vector2 spos = GetWorldToScreen((Vector3){wpos.x, 1.0f, wpos.y}, camera);
    DrawText(TextFormat("{%d, %d}", chunk.coord.col, chunk.coord.row), spos.x, spos.y, 20, BLACK);
}

void InitGameplayScreen() {

    framesCounter = 0;
    finishScreen = 0;

    camera = (Camera3D) {
        .position = (Vector3){.x = 0, .y = 8.0f, .z = 8.0f},
        .target = (Vector3){0},
        .up = (Vector3){.x = 0, .y = 1.0f, .z = 0},
        .fovy = 70.0f,


        .projection = CAMERA_PERSPECTIVE
    };
    cameraOffset = (Vector3){.x = 0, .y = 8.0f, .z = 8.0f};

    // initialize player
    LoadPlayerResources();
    player = CreatePlayer();

    // initialize scene
    map = generate_chunk((Checker){0});

    load_pipes_resources();
}

void UpdateGameplayScreen() {
    const float frame_time = GetFrameTime();

    ProcessInputs(&inputs);

    // process mouse inputs here
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        const Tile *tile = get_chunk_tile(&map, axial_to_checker(inputs.selected_tile));
        if (!(tile->flags & (TF_CAN_BUILD | TF_SOURCE))) { return; }

        //Start the pipe tool
        const Vector2 position = AxialToPosition(inputs.selected_tile);
        const AxialDirection dir = ((int)roundf(Vector2LineAngle(inputs.mouse_position, position) / (PI / 3)) + 2) % HD_COUNT;

        mouse_pipe_tool = true;
        pipe_tool_start_bpp = (struct BPP){
            .position = position,
            .rotation = dir,
            .id = PIPE_WELL_OPEN
        };
        pipe_tool_end_bpp.id = PIPE_NONE;
        previous_selected_tile = inputs.selected_tile;
    }
    if (mouse_pipe_tool) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            mouse_pipe_tool = false;
            // TODO: set the blueprint...
        } else {
            if (!AxialEqual(inputs.selected_tile, previous_selected_tile)) {
                Tile *tile = get_chunk_tile(&map, axial_to_checker(inputs.selected_tile));
                const int distance = AxialDistance(previous_selected_tile, inputs.selected_tile);
                if (tile->flags == 0 && distance == 1) {
                    const AxialDirection direction = AxialDirectionToward(AxialSubtract(previous_selected_tile, inputs.selected_tile));

                    if (pipe_tool_end_bpp.id == PIPE_NONE) {
                        // set the end
                        pipe_tool_end_bpp = (struct BPP){
                            .position = AxialToPosition(inputs.selected_tile),
                            // .rotation = (direction + 3) % HD_COUNT,
                            .rotation = direction,
                            .id = PIPE_SHORT_END
                        };
                        // update the well
                        pipe_tool_start_bpp.id = PIPE_WELL_CONNECTED;
                        pipe_tool_start_bpp.rotation = direction;
                    } else {
                        struct BPP bpp = {.position = pipe_tool_end_bpp.position };
                        const AxialDirection dd = ((direction - pipe_tool_end_bpp.rotation) + 6) % 6;
                        TraceLog(LOG_DEBUG, "dd: %d", dd);
                        switch (dd) {
                            case 1:
                                bpp.rotation = (pipe_tool_end_bpp.rotation + 4) % 6;
                                bpp.id = PIPE_BEND;
                                break;
                            case 2:
                                bpp.rotation = (pipe_tool_end_bpp.rotation + 3) % 6;
                                bpp.id = PIPE_SPLIT_BRANCH;
                                break;
                            case 4:
                                bpp.rotation = (pipe_tool_end_bpp.rotation + 4) % 6;
                                bpp.id = PIPE_SPLIT_BRANCH;
                                break;
                            case 5:
                                bpp.rotation = pipe_tool_end_bpp.rotation;
                                bpp.id = PIPE_BEND;
                                break;
                            default:
                                bpp.rotation = pipe_tool_end_bpp.rotation;
                                bpp.id = PIPE_STRAIGHT;
                                break;
                        }
                        arrput(pipe_tool_bpp_list, bpp);
                        tile->flags |= TF_BLUEPRINT;

                        pipe_tool_end_bpp.position = AxialToPosition(inputs.selected_tile);
                        pipe_tool_end_bpp.rotation = direction;
                    }
                    previous_selected_tile = inputs.selected_tile;
                }
            }
        }
    }


    switch (play_mode) {
        case PLAYMODE_DEFAULT:
            if (inputs.interact_select) {
                const Tile *cell = get_chunk_tile(&map, axial_to_checker(AxialAdd(player.coordinate, hexDirections[player.target_direction])));
                if (cell != nullptr) {
                    if ((cell->flags & TF_CAN_INTERACT) != 0) {
                        if ((cell->flags & TF_CAN_BUILD) != 0) {
                            play_mode = PLAYMODE_BUILD;
                            blueprint = PIPE_WELL_OPEN;
                            stop_player(&player);
                        }
                    }
                }
            }
            if (inputs.toggle_build) {
                show_build_menu = !show_build_menu;
                play_mode = PLAYMODE_BUILD_MENU;
                stop_player(&player);
            }

            // update player
            MovePlayer(&player, inputs.move_vector, frame_time);
            break;
        case PLAYMODE_BUILD: //TODO: move this mode to the player state
            MovePlayer(&player, inputs.move_vector, frame_time);

            if (inputs.rotate) { blueprint_rot = (blueprint_rot + 1) % 6; }
            if (inputs.interact_select) {
                // build the blueprint

                const Axial player_target = AxialAdd(player.coordinate, hexDirections[player.target_direction]);
                instantiate_pipe(blueprint, player_target, blueprint_rot);

                play_mode = PLAYMODE_DEFAULT;
            }

            break;
        case PLAYMODE_BUILD_MENU:
            if (inputs.interact_select) {
                show_build_menu = false;
                play_mode = PLAYMODE_BUILD;
                // blueprint = BUILDING_WELL;
                blueprint = build_menu_cursor;
            }
            if (inputs.close) {
                show_build_menu = false;
                play_mode = PLAYMODE_DEFAULT;
            }
            if (inputs.toggle_build) {
                show_build_menu = false;
                play_mode = PLAYMODE_DEFAULT;
            }
            if (inputs.h != 0) {
                build_menu_cursor += inputs.h;
            }
            if (inputs.v != 0) {
                build_menu_cursor += inputs.v * 2;
            }
            build_menu_cursor %= PIPE_COUNT;
            build_menu_cursor = (build_menu_cursor < 0) ? PIPE_COUNT - 1 : build_menu_cursor;
            break;
        default:
            // TODO: manage error, we should always be in a play mode
            break;
    }

    // update player
    UpdatePlayer(&player, frame_time);

    selectedCell = inputs.selected_tile;

    // update camera
    const Vector2 playerPosition = player.position;
    camera.target = Vector3Lerp(camera.target, (Vector3){playerPosition.x, 0, playerPosition.y}, CAMERA_SPEED * frame_time);
    camera.position = Vector3Add(camera.target, cameraOffset);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen() {
    BeginMode3D(camera);

    draw_chunk_grid(map, DARKBLUE);
    DrawChunkBoundaries(map.coord, 0.1f, BLACK);
    DrawHexWire(player.coordinate, 0.1f, BLUE); // draw actual player coordinate

    // TODO: build a display setup for quick rendering?
    for (int c = 0; c < CHUNK_SIZE; ++c) {
        for (int r = c % 2; r < CHUNK_SIZE; r += 2) {
            if ((map.layers[0][CHECKER2INDEX(c, r)].flags & TF_CAN_INTERACT) != 0) {
                const Vector2 position = CheckerToPosition((Checker){c, r});
                DrawCube((Vector3){position.x, 0, position.y}, 0.5f, 0.5f, 0.5f, RED);
            }
        }
    }

    // Pipes
    draw_pipes();
    // MOUSE BLUEPRINT MODE
    // draw the blueprint pipe network
    draw_pipe_wire(pipe_tool_start_bpp.id, pipe_tool_start_bpp.position, pipe_tool_start_bpp.rotation, SKYBLUE);
    const int bpp_count = arrlen(pipe_tool_bpp_list);
    for (int i = 0; i < bpp_count; ++i) {
        draw_pipe_wire(pipe_tool_bpp_list[i].id, pipe_tool_bpp_list[i].position, pipe_tool_bpp_list[i].rotation, SKYBLUE);
    }
    if (pipe_tool_end_bpp.id != PIPE_NONE) {
        draw_pipe_wire(pipe_tool_end_bpp.id, pipe_tool_end_bpp.position, pipe_tool_end_bpp.rotation, SKYBLUE);
    }

    DrawHex(selectedCell, -0.2f, ORANGE);
    DrawHex(AxialAdd(player.coordinate, hexDirections[player.target_direction]), 0, GREEN);
    // ddraw_inputs();

    // draw player
    DrawModelEx(
        player.model,
        (Vector3){.x = player.position.x, .y = 0, .z = player.position.y},
        (Vector3){.x = 0, .y = 1.0f, .z = 0},
        player.rotation * RAD2DEG + 90,
        (Vector3){.x = 2.0f, .y = 2.0f, .z = 2.0f},
        WHITE
    );
    if (PLAYMODE_BUILD == play_mode) {
        const Axial player_target = AxialAdd(player.coordinate, hexDirections[player.target_direction]);
        const Vector2 blueprint_pos = AxialToPosition(player_target);
        const Tile *t = get_chunk_tile(&map,axial_to_checker(player_target));
        draw_pipe_wire(blueprint, blueprint_pos, blueprint_rot, ((nullptr == t || t->flags & TF_SOURCE) > 0) ? RED : GREEN);
    }

    EndMode3D();

    // DEBUG
    ddraw_info(10, 30);
    ddraw_chunk_info(map);
    DrawFPS(10, 10);

    if (show_build_menu) {
        // menu background
        const int x = 20, y = 20;
        DrawRectangleGradientV(x, y, 200, 400, GRAY, DARKGRAY);

        // buildable icon
        for (int i = 0; i < PIPE_COUNT; ++i) {
            const int row = i / 2;
            const int column = i % 2;

            DrawRectangleGradientV(x + 10 + 85 * column + 5 , y + 10 + 30 * row + 5, 80, 25, DARKBLUE, BLUE);
            DrawText(get_pipe_name(i), 40 + 85 * column, 40 + 30 * row, 10, WHITE);
        }

        // cursor
        DrawRectangleLines(x + 10 + (build_menu_cursor % 2) * 85 + 5, y + 10 + (build_menu_cursor / 2) * 30 + 5, 80, 25, ORANGE);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen() {
    unload_pipes_resources();
    delete_chunk(&map);
    UnloadPlayerResources();
}

// Gameplay Screen should finish?
int FinishGameplayScreen() {
    return finishScreen;
}