#version 330 core
layout (location = 0) out vec3 RSMPos;
layout (location = 1) out vec3 RSMNormal;
layout (location = 2) out vec4 RSMFlux;

in vec2 TexCoords;
in vec3 vsPosition;
in vec3 vsNormal;

uniform sampler2D texture_diffuse;
uniform float Ks;
uniform vec3 lightIntensity;

void main()
{    
    vec3 VPLFlux = texture(texture_diffuse, TexCoords).rgb;
    VPLFlux = VPLFlux * lightIntensity;
    vec3 tmp_flux_ks = lightIntensity * Ks;
    RSMFlux = vec4(VPLFlux, tmp_flux_ks);

    RSMPos = vsPosition;
    RSMNormal = normalize(vsNormal);
}