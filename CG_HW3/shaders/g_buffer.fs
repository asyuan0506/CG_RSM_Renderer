#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 vsPosition;
in vec3 vsNormal;

uniform sampler2D texture_diffuse1;
uniform float Ks;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = vsPosition;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(vsNormal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = Ks;
}