//
// Created by green on 2026-07-21.
//

#include "pipes.h"
#include "pipe_rules.h"
#include "raymath.h"
#include "raylib_ext.h"
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
static InstanceMaterialLocations default_material_instancing_locs;
static Material default_material;
static Matrix *pipe_transform_list[PIPE_COUNT];

// TODO: contextualize with a drone?
// PIPE TOOL
struct PipeToolEntry {
    Axial key;
    struct PipeToolElement {
        unsigned int rotation;
        enum PipeModelID id;
        // TODO: rename to `input_mask`
        char inputs;
    } value;
};
// static struct PipeToolEntry *pipe_tool_entry_list;
static Axial previous_tile;
// #define PIPE_TOOL_HASH(A) ((unsigned int)((A).q * 1459 + (A).r) % 32)
static struct PipeToolEntry *pipe_tool_hashmap = nullptr;

void load_pipes_resources() {
    pipe_models = LoadModel("./resources/models/pipes.glb");
    default_material_instancing = LoadMaterialDefault();
    default_material_instancing.shader = LoadShader("./resources/shaders/default_instancing_pipes.vert", "./resources/shaders/default.frag");
    default_material_instancing_locs.position_loc = GetShaderLocationAttrib(default_material_instancing.shader, "instance_position");
    default_material_instancing_locs.rotation_loc = GetShaderLocationAttrib(default_material_instancing.shader, "instance_rotation");

    pipe_color = BLUE;

    default_material = LoadMaterialDefault();

    // clear the hash_bucket_list for use with std_ds
}

void unload_pipes_resources() {
    if (pipe_tool_hashmap != nullptr) { hmfree(pipe_tool_hashmap); }
    UnloadModel(pipe_models);
}

void draw_pipes() {
    default_material_instancing.maps[MATERIAL_MAP_DIFFUSE].color = pipe_color;
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (arrlen(pipe_transform_list[i]) == 0) { continue; }
        DrawMeshInstanced(pipe_models.meshes[i], default_material_instancing, pipe_transform_list[i], arrlen(pipe_transform_list[i]));
    }
}

void draw_pipe_wire(const enum PipeModelID id, const Vector3 position, const char rotation, const Color color) {
    const Color old_color = default_material.maps[MATERIAL_MAP_DIFFUSE].color;
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = color;
    draw_mesh_wire(
        pipe_models.meshes[id],
        default_material,
        MatrixMultiply(MatrixRotateY((float)rotation * (PI / 3)), MatrixTranslate(position.x, position.y, position.z)));
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = old_color;
}

void draw_pipe(const enum PipeModelID id, const Vector3 position, const char rotation, const Color color) {
    const Color old_color = default_material.maps[MATERIAL_MAP_DIFFUSE].color;
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = color;
    DrawMesh(
        pipe_models.meshes[id],
        default_material,
        MatrixMultiply(MatrixRotateY((float)rotation * (PI / 3)), MatrixTranslate(position.x, position.y, position.z)));
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

void start_pipe_tool(const Axial start_tile, const enum PipeModelID start_pipe_id, const char hex_direction) {
    if (PIPE_NONE == start_pipe_id) {
        const struct PipeToolElement pte = {
            .rotation = hex_direction,
            .id = PIPE_WELL_OPEN,
            .inputs = 1 << hex_direction
        };
        hmput(pipe_tool_hashmap, start_tile, pte);
        previous_tile = start_tile;
    } else {
        const struct PipeToolElement pte = {
            .rotation = hex_direction,
            .id = PIPE_SPLIT_BRANCH,
            .inputs = 1 << hex_direction
        };
        hmput(pipe_tool_hashmap, start_tile, pte);
    }
    previous_tile = start_tile;
}

enum PipeModelID update_pipe_tool(const Axial next_tile, const char inputs) {
    const int distance = AxialDistance(previous_tile, next_tile);
    if (distance != 1) {
        // use line drawing or walk the tiles ?
        return PIPE_NONE;
    }

    const AxialDirection direction = AxialDirectionToward(AxialSubtract(previous_tile, next_tile));
    const AxialDirection flipped_direction = (direction + 3) % HD_COUNT;

    // Update the previous tile
    const int pi = hmgeti(pipe_tool_hashmap, previous_tile);
    if (hmlen(pipe_tool_hashmap) <= 1) {
        if (PIPE_WELL_OPEN == pipe_tool_hashmap[pi].value.id) {
            // update the well
            pipe_tool_hashmap[pi].value.id = PIPE_WELL_CONNECTED;
            pipe_tool_hashmap[pi].value.rotation = direction;
            pipe_tool_hashmap[pi].value.inputs = 1 << direction;
        }
    } else {
        const struct PipeEntry entry = pipe_ruleset[pipe_tool_hashmap[pi].value.inputs | (1 << direction)];
        if (PIPE_NONE == entry.id) { return PIPE_NONE; }
        pipe_tool_hashmap[pi].value.inputs |= 1 << direction;
        const struct PipeEntry last_entry = pipe_ruleset[pipe_tool_hashmap[pi].value.inputs];
        pipe_tool_hashmap[pi].value.id = last_entry.id;
        pipe_tool_hashmap[pi].value.rotation = last_entry.rotation;
    }

    // update the current tile
    const int ni = hmgeti(pipe_tool_hashmap, next_tile);

    struct PipeToolElement pte = {
        .rotation = direction,
        .id = PIPE_SHORT_END,
        .inputs = 1 << flipped_direction
    };
    if (ni >= 0) {
        const struct PipeEntry entry = pipe_ruleset[pipe_tool_hashmap[ni].value.inputs | (1 << direction)];
        pte.id = entry.id;
        pte.rotation = entry.rotation;
    }
    hmput(pipe_tool_hashmap, next_tile, pte);

    previous_tile = next_tile;
    return pte.id;
}

Blueprint *pipe_tool_get_blueprint_list() {
    Blueprint *bp_list = nullptr;
    for (int i = 0; i < hmlen(pipe_tool_hashmap); ++i) {
        // const Vector2 position = AxialToPosition(pipe_tool_hashmap[i].key);
        const Blueprint bp = {
            .id = pipe_tool_hashmap[i].value.id,
            .coord = pipe_tool_hashmap[i].key,
            .inputs = pipe_tool_hashmap[i].value.inputs
            // .transform  = {
            //     .position = (Vector3){.x = position.x, .y = 0.25f, .z = position.y},
            //     .rotation = (float)pipe_tool_hashmap[i].value.rotation * M_PI_3
            // }
        };
        arrput(bp_list, bp);
    }

    return bp_list;
}

void pipe_tool_clear() {
    if (pipe_tool_hashmap != nullptr) { hmfree(pipe_tool_hashmap); }
}

void draw_pipe_tool() {
    if (hmlen(pipe_tool_hashmap) <= 0) { return; }
    for (int i = 0; i < hmlen(pipe_tool_hashmap); ++i) {
        const Vector2 position = AxialToPosition(pipe_tool_hashmap[i].key);
        draw_pipe_wire(pipe_tool_hashmap[i].value.id, (Vector3){position.x, 0.25f, position.y}, pipe_tool_hashmap[i].value.rotation, SKYBLUE);
    }
}

extern Camera3D camera;
void draw_pipe_tool_debug_info() {
    if (hmlen(pipe_tool_hashmap) <= 0) { return; }

    const struct PipeToolElement pte = hmget(pipe_tool_hashmap, previous_tile);
    const Vector2 position = AxialToPosition(previous_tile);
    const Vector2 screen_position = GetWorldToScreen((Vector3){.x = position.x, .y = 1.0f, .z = position.y}, camera);
    DrawText(TextFormat("ID: %d R: %d", pte.id, pte.rotation), screen_position.x ,screen_position.y, 20, BLACK);
}


void draw_pipe_blueprint(const PipeBlueprint *blueprint) {
    for (int i = 0; i < PIPE_COUNT; ++i) {
        draw_tile_mesh_instances(
            pipe_models.meshes[i],
            blueprint->instance_lists[i],
            default_material_instancing,
            default_material_instancing_locs
        );
    }
}

void delete_pipe_blueprint(PipeBlueprint *blueprint) {
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (blueprint->instance_lists[i] != nullptr) {
            arrfree(blueprint->instance_lists[i]);
        }
    }
}
