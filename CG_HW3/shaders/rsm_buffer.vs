#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vsPosition; 
out vec2 TexCoords;
out vec3 vsNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightViewVP;

void main()
{
    vec4 FragPosInWorldSpace = worldMatrix * vec4(aPos, 1.0);
    gl_Position = lightViewVP * FragPosInWorldSpace;
    vec4 tmpPos = viewMatrix * FragPosInWorldSpace;
    vsPosition = tmpPos.xyz / tmpPos.w; 
    TexCoords = aTexCoords;

    vsNormal = (normalMatrix * vec4(aNormal, 0.0)).xyz; // In camera view space
}