#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vsPosition;
out vec2 TexCoords;
out vec3 vsNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    vec4 tmpPos = viewMatrix * worldMatrix * vec4(aPos, 1.0);
    vsPosition = tmpPos.xyz / tmpPos.w; 
    TexCoords = aTexCoords;

    vsNormal = (normalMatrix * vec4(aNormal, 0.0)).xyz;
}