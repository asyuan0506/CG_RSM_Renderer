#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

// Transformation matrix.
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;
// --------------------------------------------------------
// Add more uniform variables if needed.
// --------------------------------------------------------
// Data pass to fragment shader.
// --------------------------------------------------------
// Add your data for interpolation.
out vec2 iTexCoord;
out vec3 vsPosition;
out vec3 vsNormal;
// --------------------------------------------------------

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);
    // --------------------------------------------------------------------
    // Add your code below to implement Phong shading with texture support.
    iTexCoord = TexCoord;
    vec4 tmpPos = viewMatrix * worldMatrix * vec4(Position, 1.0);
    vsPosition = tmpPos.xyz / tmpPos.w;
    vsNormal = (normalMatrix * vec4(Normal, 0.0)).xyz;
    // --------------------------------------------------------------------
}