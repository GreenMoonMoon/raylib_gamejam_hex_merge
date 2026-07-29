#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

//in mat4 instanceTransform;
in int instance_rotation;
in vec3 instance_position;


// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

// NOTE: Add your custom variables here
#define M_PI_3 1.0471975512

void main()
{
    float cosres = cos(instance_rotation * M_PI_3);
    float sinres = sin(instance_rotation * M_PI_3);
    mat4 instanceTransform = mat4(
            vec4(cosres, 0, -sinres, instance_position.x),
            vec4(0, 1, 0, instance_position.y),
            vec4(sinres, 0, cosres, instance_position.z),
            vec4(0, 0, 0, 1)
    );

    // Send vertex attributes to fragment shader
    fragPosition = vec3(instanceTransform*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    // Calculate final vertex position, note that we multiply mvp by instanceTransform
    gl_Position = mvp*instanceTransform*vec4(vertexPosition, 1.0);
}
