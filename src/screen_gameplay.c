#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "hex.h"
#include "draw_utils.h"

#define M_PI_3 1.0471975512f

#define DEFAULT_ANIM_SPEED 24
#define CAMERA_SPEED 4.0f

#define STEP_SOUND_DELAY 0.35f

#define GIS_DRAG_MOVE 0
#define GIS_SELECT_DESTINATION 1

// TODO: move to player module
typedef enum PlayerState {
    PS_IDLE,
    PS_MOVING,
} PlayerState;

// TODO: move to input module
typedef enum InputState {
    IS_NONE,
    IS_TOUCH_DRAG,
    IS_TOUCH_SELECT,
    IS_KEYBOARD_DPAD,
} InputState;


//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

// camera
static Camera3D camera;
static Vector3 cameraOffset;

// inputs
static Vector2 moveDirection;
static int lastGesture;
static InputState inputState;
static Vector3 touchedPosition;
static HexCoord touchedCell;
static HexDirection hexMoveDir;

// player
static Model playerModel;
static ModelAnimation *playerAnimations;
static int playerAnimCount;
static int playerCurrentAnim;
static float playerAnimFrame;
static float playerAnimSpeed;
static PlayerState playerState;
static HexCoord playerCoordinate;
static Vector2 playerPosition;
static float playerRotation;

static Sound stepSound = { 0 };
static float stepSoundTimer = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static void DrawDebugInfo(const int x, const int y) {
    // animation frame
    DrawText(TextFormat("Animation frame: %.2f", playerAnimFrame), x, y, 20, DARKGRAY);

    // hex coordinate
    DrawText(TextFormat("Coordinate: q:%d  r:%d", playerCoordinate.q, playerCoordinate.r), x,
             y + 20, 20, DARKGRAY);

    // player state
    const char *state_name[] = {"IDLE", "MOVING"};
    DrawText(TextFormat("State: %s", state_name[playerState]), x, y + 40, 20, DARKGRAY);

    // player rotation
    DrawText(TextFormat("Rotation: %d", (int)(playerRotation * RAD2DEG)), x, y + 60, 20, DARKGRAY);

    // gesture input state
    DrawText(inputState == 0 ? "Drag" : "Select", x, y + 80, 20, GRAY);

    // hex direction
    DrawText(TextFormat("Hex direction: %d", hexMoveDir), x, y + 100, 20, DARKGRAY);
}

void DrawDebugInputs(void) {
    const Vector3 pos = {playerPosition.x + GRID_OFFSET_X, 2.0f, playerPosition.y + GRID_OFFSET_Y};
    DrawCircle3D(pos, 1.5f, (Vector3){1.0f, 0, 0}, 90, DARKBLUE);
    DrawLine3D( pos, (Vector3){pos.x + moveDirection.x * 1.5f, pos.y, pos.z + moveDirection.y * 1.5f}, RED );
    const float angle = (float)(hexMoveDir + 3) * M_PI_3;
    DrawLine3D( pos, Vector3Add(pos, (Vector3){sinf(-angle), 0, cosf(angle)}), BLUE );
}

// TODO: put into the input module
static void ProcessInputs(void) {
    // handle touch input
    const int currentGesture = GetGestureDetected();
    const Vector2 touchPosition = GetTouchPosition(0);

    if (currentGesture != GESTURE_NONE) {
        const Ray ray = GetScreenToWorldRay(touchPosition, camera);
        const float d = (-ray.position.y / ray.direction.y);
        touchedPosition = Vector3Add(ray.position, Vector3Scale(ray.direction, d));

        if (lastGesture == GESTURE_NONE) {
            // get the cell coordinate from the touch position
            touchedCell = PositionToHexCoord((Vector2){touchedPosition.x - GRID_OFFSET_X, touchedPosition.z - GRID_OFFSET_Y});
            inputState = HexCoordEqual(touchedCell, playerCoordinate) ? IS_TOUCH_DRAG : IS_TOUCH_SELECT;
        }

        switch (inputState) {
            case  IS_TOUCH_DRAG:
                moveDirection = Vector2Normalize(Vector2Subtract((Vector2){touchedPosition.x, touchedPosition.z}, playerPosition));
                break;
            case IS_TOUCH_SELECT:
                // TODO: implement move command and pathing
                break;
            default: break;
        }
    } else {
        if (inputState == IS_TOUCH_DRAG) {
            moveDirection = (Vector2){0};
            inputState = IS_NONE;
        }
    }
    lastGesture = currentGesture;

    // handle keyboard input
    Vector2 moveInput = {
        IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };
    if (fabsf(moveInput.x) > EPSILON || fabsf(moveInput.y) > EPSILON) {
        inputState = IS_KEYBOARD_DPAD;
        moveInput = Vector2Normalize(moveInput);
    }

    if (inputState == IS_KEYBOARD_DPAD) {
        // FIXME: replace with a radial lerp?
        moveDirection = Vector2Lerp(moveDirection, moveInput, 0.25f);
    }

    if (IsKeyPressed(KEY_PAUSE)) {
        TraceLog(LOG_DEBUG, "PAUSE KEY PRESSED");
    }
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
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

    Texture colormap = LoadTexture("resources/textures/colormap.png");

    playerModel = LoadModel("resources/models/character_female_b.glb");
    playerModel.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = colormap;
    playerAnimations = LoadModelAnimations("resources/models/character_female_a.glb", &playerAnimCount);
    playerCurrentAnim = 1;
    playerAnimFrame = 0;
    playerAnimSpeed = DEFAULT_ANIM_SPEED;
    playerCoordinate = (HexCoord){0, 0}; // hex coordinate component sum is always 0
    playerPosition = HexCoordToPosition(playerCoordinate);
    playerRotation = PI;

    // SFX
    stepSound = LoadSound("resources/sfx/step.wav");

    // initialize scene

}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    const float frameTime = GetFrameTime();

    ProcessInputs();

    // // TODO: buffer 2 or 3 frame inputs, then calculate the angle and divide by 6 directions to use in a switch statement
#define IS_MOVING_CUTOFF 0.1f
    const bool isMoving = Vector2LengthSqr(moveDirection) > IS_MOVING_CUTOFF;
    if (isMoving) {
        // get the hex direction
        hexMoveDir = (int)roundf(atan2f(-moveDirection.x, moveDirection.y) / M_PI_3) + 3;
    }
    // // get inputs
    // const bool moveUp = IsKeyDown(KEY_W);
    // const bool moveDown = IsKeyDown(KEY_S);
    // const bool moveLeft = IsKeyDown(KEY_A);
    // const bool moveRight = IsKeyDown(KEY_D);
    // const char anyInputs = moveUp | moveDown << 1 | moveLeft << 2 | moveRight << 3;

    // manage player state machine
#define MOVE_TIME 0.25f
    static Vector2 lastPlayerPosition;
    static Vector2 nextPlayerPosition;
    // static int angle;
    static float moveFrame;

    float angle = playerRotation;
    switch (playerState) {
        case PS_MOVING:
            stepSoundTimer -= frameTime;
            if (stepSoundTimer <= 0) {
                PlaySound(stepSound);
                stepSoundTimer = STEP_SOUND_DELAY;
            }
            if (moveFrame < MOVE_TIME) {
                moveFrame += frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);
            } else if (isMoving) {
                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                // if (moveDown && !moveRight) { playerCoordinate.q++; }
                // if (moveUp && !moveLeft) { playerCoordinate.q--; }
                // if (moveRight) { playerCoordinate.r++; }
                // if (moveLeft) { playerCoordinate.r--; }

                nextPlayerPosition = HexCoordToPosition(playerCoordinate);
                moveFrame = frameTime;
                playerPosition = Vector2Lerp(lastPlayerPosition, nextPlayerPosition, moveFrame / MOVE_TIME);

                // player angle
                // angle = (int)floorf(Vector2Angle(lastPlayerPosition, nextPlayerPosition) / 6);
                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            } else {
                // switch to idle
                playerState = PS_IDLE;
                playerCurrentAnim = 1;
                playerAnimSpeed = DEFAULT_ANIM_SPEED;
                // playerAnimFrame = 0; // no need to reset the idle animation
                moveFrame = 0;
            }
            break;
        case PS_IDLE:
            if (isMoving) {
                // switch to moving
                playerState = PS_MOVING;
                playerCurrentAnim = 3;
                playerAnimSpeed = 48; // double speed
                playerAnimFrame = 0;

                lastPlayerPosition = HexCoordToPosition(playerCoordinate);

                // if (moveDown && !moveRight) { playerCoordinate.q++; }
                // if (moveUp && !moveLeft) { playerCoordinate.q--; }
                // if (moveRight) { playerCoordinate.r++; }
                // if (moveLeft) { playerCoordinate.r--; }

                nextPlayerPosition = HexCoordToPosition(playerCoordinate);

                angle = Vector2LineAngle(lastPlayerPosition, nextPlayerPosition);
            }
            break;
    }

    // update player model
    // -- update orientation
    playerRotation = angle;
    // -- update animation
    playerAnimFrame = fmodf(
        playerAnimFrame + frameTime * playerAnimSpeed,
        (float)playerAnimations[playerCurrentAnim].keyframeCount
    );
    UpdateModelAnimation(playerModel, playerAnimations[playerCurrentAnim], playerAnimFrame);


    // update camera
    camera.target = Vector3Lerp(camera.target, (Vector3){playerPosition.x, 0, playerPosition.y}, CAMERA_SPEED * frameTime);
    camera.position = Vector3Add(camera.target, cameraOffset);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    // Vector2 pos = { 20, 10 };
    // DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    // DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

    BeginMode3D(camera);

    DrawHexGrid(20, 10);
    DrawHexWire(playerCoordinate, 0.1f, BLUE); // draw actual player coordinate

    // DEBUG
    DrawHex(touchedCell, -0.2f, RED);
    DrawDebugInputs();

    // draw player
    // DrawModel(playerModel, (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y} , 2.0f, WHITE);
    DrawModelEx(
        playerModel,
        (Vector3){playerPosition.x + GRID_OFFSET_X, 0, playerPosition.y + GRID_OFFSET_Y},
        (Vector3){0, 1.0f, 0},
        playerRotation * RAD2DEG + 90,
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
    UnloadSound(stepSound);

    UnloadModelAnimations(playerAnimations, playerAnimCount);
    UnloadModel(playerModel);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}