//
// Created by green on 2026-07-21.
//

#include "pipes.h"
#include "raymath.h"
#include "draw_utils.h"
#include "extern/stb_ds.h"

static const char *pipe_names[PIPE_COUNT] = {
    "Bend",
    "dir valve mount",
    "end",
    "5 split",
    "joint",
    "4 split",
    "well",
    "6 split",
    "straight",
    "3 split",
    "joint valve mount",
    "w split",
    "x split",
    "y split",
    "valve"
};

static Color pipe_color;
static Model pipe_models;
static Material default_material_instancing;
static Material default_material;
static Matrix *pipe_transform_list[PIPE_COUNT];

// TODO: contextualize with a drone?
// PIPE TOOL
struct BPP {
    Vector2 position;
    unsigned int rotation;
    enum PipeModelID id;
};
static struct BPP pipe_tool_start_bpp, pipe_tool_end_bpp;
static struct BPP *pipe_tool_bpp_list;
static Axial previous_tile;

void load_pipes_resources() {
    pipe_models = LoadModel("./resources/models/pipes.glb");
    default_material_instancing = LoadMaterialDefault();
    default_material_instancing.shader = LoadShader("./resources/shaders/default_instancing.vert", "./resources/shaders/default.frag");
    pipe_color = BLUE;

    default_material = LoadMaterialDefault();
}

void unload_pipes_resources() {
    if (pipe_tool_bpp_list != nullptr) { arrfree(pipe_tool_bpp_list); }
    UnloadModel(pipe_models);
}

void draw_pipes() {
    default_material_instancing.maps[MATERIAL_MAP_DIFFUSE].color = pipe_color;
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (arrlen(pipe_transform_list[i]) == 0) { continue; }
        DrawMeshInstanced(pipe_models.meshes[i], default_material_instancing, pipe_transform_list[i], arrlen(pipe_transform_list[i]));
    }
}

void draw_pipe_wire(const enum PipeModelID id, const Vector2 position, const char rotation, const Color color) {
    const Color old_color = default_material.maps[MATERIAL_MAP_DIFFUSE].color;
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = color;
    draw_mesh_wire(
        pipe_models.meshes[id],
        default_material,
        MatrixMultiply(MatrixRotateY((float)rotation * (PI / 3)), MatrixTranslate(position.x, 0.25f, position.y)));
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = old_color;
}

void instantiate_pipe(const enum PipeModelID id, const Axial coord, const char rotation) {
    const Vector2 position = AxialToPosition(coord);
    const Matrix transform = MatrixMultiply(
        MatrixRotateY((float)rotation * (PI / 3)),
        MatrixTranslate(position.x, 0.25f, position.y)
    );
    arrput(pipe_transform_list[id], transform);
}

const char * get_pipe_name(const enum PipeModelID id) {
    return pipe_names[id];
}

void start_pipe_tool(const Axial start_tile, const char hex_direction) {
    //Start the pipe tool
    const Vector2 position = AxialToPosition(start_tile);

    pipe_tool_start_bpp = (struct BPP){
        .position = position,
        .rotation = hex_direction,
        .id = PIPE_WELL_OPEN
    };
    pipe_tool_end_bpp.id = PIPE_NONE;
    previous_tile = start_tile;
}

bool update_pipe_tool(const Axial next_tile) {
    const int distance = AxialDistance(previous_tile, next_tile);
    if (distance != 1) { return false; }

    const AxialDirection direction = AxialDirectionToward(AxialSubtract(previous_tile, next_tile));
    const AxialDirection dd = ((direction - pipe_tool_end_bpp.rotation) + 6) % 6;
    if (dd == 4 || dd == 2) { return false; }
    if (pipe_tool_end_bpp.id == PIPE_NONE) {
        // set the end
        pipe_tool_end_bpp = (struct BPP){
            .position = AxialToPosition(next_tile),
            .rotation = direction,
            .id = PIPE_SHORT_END
        };
        // update the well
        pipe_tool_start_bpp.id = PIPE_WELL_CONNECTED;
        pipe_tool_start_bpp.rotation = direction;
    } else {
        struct BPP bpp = {.position = pipe_tool_end_bpp.position };
        switch (dd) {
            case 1:
                bpp.rotation = (pipe_tool_end_bpp.rotation + 4) % 6;
                bpp.id = PIPE_BEND;
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

        pipe_tool_end_bpp.position = AxialToPosition(next_tile);
        pipe_tool_end_bpp.rotation = direction;
    }

    previous_tile = next_tile;
    return true;
}

Blueprint commit_pipe_tool() {
    Blueprint bp = create_blueprint();
    arrput(bp.bpp_list, pipe_tool_start_bpp);
    for (int i = 0; i < arrlen(pipe_tool_bpp_list); ++i) { arrput(bp.bpp_list, pipe_tool_bpp_list[i]); }
    if (pipe_tool_end_bpp.id != PIPE_NONE) { arrput(bp.bpp_list, pipe_tool_end_bpp); }

    // clear pipe_tool
    arrsetlen(pipe_tool_bpp_list, 0);

    return bp;
}

void draw_pipe_tool() {
    draw_pipe_wire(pipe_tool_start_bpp.id, pipe_tool_start_bpp.position, pipe_tool_start_bpp.rotation, SKYBLUE);
    const int bpp_count = arrlen(pipe_tool_bpp_list);
    for (int i = 0; i < bpp_count; ++i) {
        draw_pipe_wire(pipe_tool_bpp_list[i].id, pipe_tool_bpp_list[i].position, pipe_tool_bpp_list[i].rotation, SKYBLUE);
    }
    if (pipe_tool_end_bpp.id != PIPE_NONE) {
        draw_pipe_wire(pipe_tool_end_bpp.id, pipe_tool_end_bpp.position, pipe_tool_end_bpp.rotation, SKYBLUE);
    }
}

void draw_pipe_blueprint(const Blueprint blueprint) {
    for (int i = 0; i < arrlen(blueprint.bpp_list); ++i) {
        draw_pipe_wire(blueprint.bpp_list[i].id, blueprint.bpp_list[i].position, blueprint.bpp_list[i].rotation, SKYBLUE);
    }
}
