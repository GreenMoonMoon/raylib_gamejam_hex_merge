//
// Created by green on 2026-07-21.
//

#include "pipes.h"
#include "raymath.h"
#include "draw_utils.h"
#include "extern/stb_ds.h"

const char *pipe_names[PIPE_COUNT] = {
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

void load_pipes_resources() {
    pipe_models = LoadModel("./resources/models/pipes.glb");
    default_material_instancing = LoadMaterialDefault();
    default_material_instancing.shader = LoadShader("./resources/shaders/default_instancing.vert", "./resources/shaders/default.frag");
    pipe_color = BLUE;

    default_material = LoadMaterialDefault();
}

void unload_pipes_resources() {
    UnloadModel(pipe_models);
}

void draw_pipes() {
    default_material_instancing.maps[MATERIAL_MAP_DIFFUSE].color = pipe_color;
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (arrlen(pipe_transform_list[i]) == 0) { continue; }
        DrawMeshInstanced(pipe_models.meshes[i], default_material_instancing, pipe_transform_list[i], arrlen(pipe_transform_list[i]));
    }
}

void draw_pipe_wire(enum PipeModelID id, Vector2 position, int rotation, Color color) {
    const Color old_color = default_material.maps[MATERIAL_MAP_DIFFUSE].color;
    // default_material.maps[MATERIAL_MAP_DIFFUSE].color = ((nullptr == t || t->flags & TF_SOURCE) > 0) ? RED : GREEN;
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = color;
    draw_mesh_wire(
        pipe_models.meshes[id],
        default_material,
        MatrixMultiply(MatrixRotateY(rotation * (PI / 3)), MatrixTranslate(position.x, 0.25f, position.y)));
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = old_color;
}

void instantiate_pipe(enum PipeModelID id, Axial coord, int rotation) {
    const Vector2 position = AxialToPosition(coord);
    const Matrix transform = MatrixMultiply(
        MatrixRotateY(rotation * (PI / 3)),
        MatrixTranslate(position.x, 0.25f, position.y)
    );
    arrput(pipe_transform_list[id], transform);
}

const char * get_pipe_name(enum PipeModelID id) {
    return pipe_names[id];
}
