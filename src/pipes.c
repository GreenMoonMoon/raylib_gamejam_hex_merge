//
// Created by green on 2026-07-21.
//

#include "pipes.h"
#include "raymath.h"
#include "rlgl.h"
#include "draw_utils.h"
#include "extern/stb_ds.h"

struct PipeTransform {
    Vector3 position;
    float rotation;
};

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
int default_material_instancing_pos_loc;
int default_material_instancing_rot_loc;
static Material default_material;
static Matrix *pipe_transform_list[PIPE_COUNT];

// TODO: contextualize with a drone?
// PIPE TOOL
struct BPP {
    // Vector2 position;
    Axial coordinate;
    unsigned int rotation;
    enum PipeModelID id;
};
static struct BPP pipe_tool_start_bpp;
static struct BPP *pipe_tool_bpp_list;
static Axial previous_tile;

void load_pipes_resources() {
    pipe_models = LoadModel("./resources/models/pipes.glb");
    default_material_instancing = LoadMaterialDefault();
    default_material_instancing.shader = LoadShader("./resources/shaders/default_instancing_pipes.vert", "./resources/shaders/default.frag");
    default_material_instancing_pos_loc = GetShaderLocationAttrib(default_material_instancing.shader, "instance_position");
    default_material_instancing_rot_loc = GetShaderLocationAttrib(default_material_instancing.shader, "instance_rotation");

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

void draw_pipe_wire(const enum PipeModelID id, const Vector3 position, const char rotation, const Color color) {
    const Color old_color = default_material.maps[MATERIAL_MAP_DIFFUSE].color;
    default_material.maps[MATERIAL_MAP_DIFFUSE].color = color;
    draw_mesh_wire(
        pipe_models.meshes[id],
        default_material,
        MatrixMultiply(MatrixRotateY((float)rotation * (PI / 3)), MatrixTranslate(position.x, position.y + 0.25f, position.z)));
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
    // const Vector2 position = AxialToPosition(start_tile);

    pipe_tool_start_bpp = (struct BPP){
        // .position = position,
        .coordinate = start_tile,
        .rotation = hex_direction,
        .id = PIPE_WELL_OPEN
    };
    previous_tile = start_tile;
}

bool update_pipe_tool(const Axial next_tile) {
    const int distance = AxialDistance(previous_tile, next_tile);
    if (distance != 1) { return false; }

    const AxialDirection direction = AxialDirectionToward(AxialSubtract(previous_tile, next_tile));
    if (arrlen(pipe_tool_bpp_list) == 0) {
        // set the end
        const struct BPP bpp = {
            // .position = AxialToPosition(next_tile),
            .coordinate = next_tile,
            .rotation = direction,
            .id = PIPE_SHORT_END
        };
        arrput(pipe_tool_bpp_list, bpp);
        // update the well
        pipe_tool_start_bpp.id = PIPE_WELL_CONNECTED;
        pipe_tool_start_bpp.rotation = direction;
    } else {
        const AxialDirection dd = ((direction - arrlast(pipe_tool_bpp_list).rotation) + 6) % 6;
        if (dd == 4 || dd == 2) { return false; }
        switch (dd) {
            case 1:
                arrlast(pipe_tool_bpp_list).rotation = (arrlast(pipe_tool_bpp_list).rotation + 4) % 6;
                arrlast(pipe_tool_bpp_list).id = PIPE_BEND;
                break;
            case 5:
                arrlast(pipe_tool_bpp_list).id = PIPE_BEND;
                break;
            default:
                arrlast(pipe_tool_bpp_list).id = PIPE_STRAIGHT;
                break;
        }

        const struct BPP bpp  = {
            // .position = AxialToPosition(next_tile),
            .coordinate = next_tile,
            .rotation = direction,
            .id = PIPE_SHORT_END
        };
        arrput(pipe_tool_bpp_list, bpp);
    }

    previous_tile = next_tile;
    return true;
}

void commit_pipe_tool_to_blueprints(PipeBlueprint *blueprints) {
    const Vector2 start_position = AxialToPosition(pipe_tool_start_bpp.coordinate);
    const struct PipeTransform start_transform = {
        .position = (Vector3){.x = start_position.x, .y = 0.25f, .z = start_position.y},
        .rotation = (float)pipe_tool_start_bpp.rotation * M_PI_3
    };
    arrput(blueprints->instance_lists[pipe_tool_start_bpp.id], start_transform);
    for (int i = 0; i < arrlen(pipe_tool_bpp_list); ++i) {
        const struct Vector2 position = AxialToPosition(pipe_tool_bpp_list[i].coordinate);
        const struct PipeTransform transform = {
            .position = (Vector3){.x = position.x, .y = 0.25f, .z = position.y},
            .rotation = (float)pipe_tool_bpp_list[i].rotation * M_PI_3
        };
        arrput(blueprints->instance_lists[pipe_tool_bpp_list[i].id], transform);
    }

    // clear pipe_tool
    arrsetlen(pipe_tool_bpp_list, 0);
}

void draw_pipe_tool() {
    const Vector2 start_position = AxialToPosition(pipe_tool_start_bpp.coordinate);
    draw_pipe_wire(pipe_tool_start_bpp.id, (Vector3){start_position.x, 0, start_position.y}, pipe_tool_start_bpp.rotation, SKYBLUE);
    const int bpp_count = arrlen(pipe_tool_bpp_list);
    for (int i = 0; i < bpp_count; ++i) {
        const Vector2 position = AxialToPosition(pipe_tool_bpp_list[i].coordinate);
        draw_pipe_wire(pipe_tool_bpp_list[i].id, (Vector3){position.x, 0, position.y}, pipe_tool_bpp_list[i].rotation, SKYBLUE);
    }
}

static void draw_pipe_instances(const enum PipeModelID id, const struct PipeTransform *transform_list, const Material material) {
    const Mesh mesh = pipe_models.meshes[id];

    rlEnableShader(material.shader.id);


    // Send required data to shader (matrices, values)
    //-----------------------------------------------------
    // Upload to shader material.colDiffuse
    if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1) {
        const float values[4] = {
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.r/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.g/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.b/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.a/255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
    }

    // Upload to shader material.colSpecular (if location available)
    if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
    {
        float values[4] = {
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.r/255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.g/255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.b/255.0f,
            (float)material.maps[MATERIAL_MAP_SPECULAR].color.a/255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
    }


    // in case stereo render is required, and they need to be modified
    // NOTE: At this point the modelview matrix contains the view matrix (camera)
    // That's because BeginMode3D() sets it and there is no model-drawing function
    // that modifies it, all use rlPushMatrix() and rlPopMatrix()
    Matrix matModel = MatrixIdentity();
    Matrix matView = rlGetMatrixModelview();
    Matrix matModelView = MatrixIdentity();
    Matrix matProjection = rlGetMatrixProjection();

    // Upload view and projection matrices (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
    if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);

    // Enable mesh VAO to attach new buffer
    rlEnableVertexArray(mesh.vaoId);

    const int instances = arrlen(transform_list);
    // struct PipeTransform *transforms = calloc(instances, sizeof(struct PipeTransform));


    // This could alternatively use a static VBO and either glMapBuffer() or glBufferSubData()
    // It isn't clear which would be reliably faster in all cases and on all platforms,
    // anecdotally glMapBuffer() seems quite slow (syncs) while glBufferSubData() seems
    // no faster, since all the transform matrices are transferred anyway
    unsigned int instances_vbo_id = rlLoadVertexBuffer(transform_list, instances * sizeof(struct PipeTransform), false);

    if (default_material_instancing_pos_loc != -1) {
        rlEnableVertexAttribute(default_material_instancing_pos_loc);
        rlSetVertexAttribute(default_material_instancing_pos_loc, 3, RL_FLOAT, 0, sizeof(struct PipeTransform), 0);
        rlSetVertexAttributeDivisor(default_material_instancing_pos_loc, 1);
    }
    if (default_material_instancing_rot_loc != -1) {
        rlEnableVertexAttribute(default_material_instancing_rot_loc);
        rlSetVertexAttribute(default_material_instancing_rot_loc, 1, RL_FLOAT, 0, sizeof(struct PipeTransform), sizeof(Vector3));
        rlSetVertexAttributeDivisor(default_material_instancing_rot_loc, 1);
    }

    rlDisableVertexBuffer();
    rlDisableVertexArray();


    // Accumulate internal matrix transform (push/pop) and view matrix
    // NOTE: In this case, model instance transformation must be computed in the shader
    matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

    // Upload model normal matrix (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
    //-----------------------------------------------------

    // Bind active texture maps (if available)
#define MAX_MATERIAL_MAPS 12
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            // Select current shader texture slot
            rlActiveTextureSlot(i);

            // Enable texture for active slot
            if ((i == MATERIAL_MAP_IRRADIANCE) ||
                (i == MATERIAL_MAP_PREFILTER) ||
                (i == MATERIAL_MAP_CUBEMAP)) rlEnableTextureCubemap(material.maps[i].texture.id);
            else rlEnableTexture(material.maps[i].texture.id);

            rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
        }
    }

    // Try binding vertex array objects (VAO)
    // or use VBOs if not possible
    if (!rlEnableVertexArray(mesh.vaoId))
    {
        // Bind mesh VBO data: vertex position (shader-location = 0)
        rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION]);
        rlSetVertexAttribute(default_material_instancing.shader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

        // Bind mesh VBO data: vertex texcoords (shader-location = 1)
        rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD]);
        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

        if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
        {
            // Bind mesh VBO data: vertex normals (shader-location = 2)
            rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
        }

        // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
        {
            if (mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR] != 0)
            {
                rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR]);
                rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, 1, 0, 0);
                rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
            else
            {
                // Set default value for unused attribute
                // NOTE: Required when using default shader and no VAO support
                float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                rlSetVertexAttributeDefault(material.shader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                rlDisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
        }

        // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
        }

        // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
        }
        if (mesh.indices != NULL) rlEnableVertexBufferElement(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES]);
    }

    int eyeCount = 1;
    if (rlIsStereoRenderEnabled()) eyeCount = 2;

    for (int eye = 0; eye < eyeCount; eye++)
    {
        // Calculate model-view-projection matrix (MVP)
        Matrix matModelViewProjection = MatrixIdentity();
        if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
        else
        {
            // Setup current eye viewport (half screen width)
            rlViewport(eye*rlGetFramebufferWidth()/2, 0, rlGetFramebufferWidth()/2, rlGetFramebufferHeight());
            matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
        }

        // Send combined model-view-projection matrix to shader
        rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

        // Draw mesh instanced
        if (mesh.indices != NULL) rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount*3, 0, instances);
        else rlDrawVertexArrayInstanced(0, mesh.vertexCount, instances);
    }

    // Unbind all bound texture maps
    for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    {
        if (default_material_instancing.maps[i].texture.id > 0)
        {
            // Select current shader texture slot
            rlActiveTextureSlot(i);

            // Disable texture for active slot
            if ((i == MATERIAL_MAP_IRRADIANCE) ||
                (i == MATERIAL_MAP_PREFILTER) ||
                (i == MATERIAL_MAP_CUBEMAP)) rlDisableTextureCubemap();
            else rlDisableTexture();
        }
    }

    // Disable all possible vertex array objects (or VBOs)
    rlDisableVertexArray();
    rlDisableVertexBuffer();
    rlDisableVertexBufferElement();

    // Disable shader program
    rlDisableShader();

    // Remove instance transforms buffe    // MatrixTranslate()
    // MatrixRotateY()
    // MatrixMultiply()

    rlUnloadVertexBuffer(instances_vbo_id);
    // RL_FREE(instanceTransform);
}

void draw_pipe_blueprint(const PipeBlueprint *blueprint) {
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (arrlen(blueprint->instance_lists[i]) > 2) {
            draw_pipe_instances(i, blueprint->instance_lists[i], default_material_instancing);
        } else {
            for (int j = 0; j < arrlen(blueprint->instance_lists[i]); ++j) {
                draw_pipe_wire(i, blueprint->instance_lists[i][j].position, blueprint->instance_lists[i][j].rotation, SKYBLUE);
            }
        }
    }
}

void delete_pipe_blueprint(PipeBlueprint *blueprint) {
    for (int i = 0; i < PIPE_COUNT; ++i) {
        if (blueprint->instance_lists[i] != nullptr) {
            arrfree(blueprint->instance_lists[i]);
        }
    }
}
