#version 330 core
out vec4 FragColor;

in vec2 iTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
// Light data.
const int POINT_LIGHT_CNT = 1;
const int SPOT_LIGHT_CNT = 3;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos[POINT_LIGHT_CNT];
uniform vec3 pointLightIntensity[POINT_LIGHT_CNT];
uniform vec3 spotLightPosition[SPOT_LIGHT_CNT];
uniform vec3 spotLightDirection[SPOT_LIGHT_CNT];
uniform vec3 spotLightIntensity[SPOT_LIGHT_CNT];
uniform float spotLightCutoffStartInDegree[SPOT_LIGHT_CNT];
uniform float spotLightTotalWidthInDegree[SPOT_LIGHT_CNT];

uniform mat4 viewMatrix;

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(float Ks, vec3 I, vec3 P, vec3 N, vec3 lightDir, float Ns)
{
    // Try to implement yourself!
    vec3 viewDir = normalize(-P); // View direction in view space.
    vec3 VH = normalize(lightDir + viewDir); // View half vector.
    return Ks * I * pow(max(0, dot(N, VH)), Ns);
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, iTexCoord).rgb;
    vec3 Normal = texture(gNormal, iTexCoord).rgb;
    vec3 Diffuse_color = texture(gAlbedoSpec, iTexCoord).rgb;
    float Specular_color = texture(gAlbedoSpec, iTexCoord).a;
    
    // then calculate lighting as usual
    vec3 lighting = Diffuse_color * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(-FragPos);
    // Directional light
    vec3 vsLightDir = (viewMatrix * vec4(-dirLightDir, 0.0)).xyz;
    vsLightDir = normalize(vsLightDir);
    // Diffuse.
    vec3 diffuse = Diffuse(Diffuse_color, dirLightRadiance, Normal, vsLightDir);
    // Specular.
    vec3 specular = Specular(Specular_color, dirLightRadiance, FragPos, Normal, vsLightDir, 16.0);
    vec3 dirLight = diffuse + specular;
    // Point light.
    vec3 pointLight = vec3(0.0);
    for(int i = 0; i < POINT_LIGHT_CNT; i++) {
        vec4  tmpPos = viewMatrix * vec4(pointLightPos[i], 1.0);
        vec3  vsLightPos = tmpPos.xyz / tmpPos.w;
        vsLightDir = normalize(vsLightPos - FragPos);
        float distSurfaceToLight = distance(vsLightPos, FragPos);
        float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
        vec3 radiance = pointLightIntensity[i] * attenuation;
        // Diffuse.
        diffuse = Diffuse(Diffuse_color, radiance, Normal, vsLightDir);
        // Specular.
        specular = Specular(Specular_color, radiance, FragPos, Normal, vsLightDir, 16.0); // Hard-coded shininess.
        pointLight += diffuse + specular;
    }
    // Spot light.
    vec3 spotLight = vec3(0.0);
    for (int i = 0; i < SPOT_LIGHT_CNT; i++) {
        vec4 tmpPos = viewMatrix * vec4(spotLightPosition[i], 1.0);
        vec3 vsLightPos = tmpPos.xyz / tmpPos.w;
        vsLightDir = normalize(vsLightPos - FragPos);  // Light direction from surface to light.
        vec3 vsSpotDir = (viewMatrix * vec4(-spotLightDirection[i], 0.0)).xyz; // Reverse the direction.
        vsSpotDir = normalize(vsSpotDir);
        float distSurfaceToLight = distance(vsLightPos, FragPos);
        float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
        float cosA = dot(vsSpotDir, vsLightDir);  // A, T, F explained in the PPT.
        float cosF = cos(radians(spotLightCutoffStartInDegree[i]));
        float cosT = cos(radians(spotLightTotalWidthInDegree[i]));
              attenuation *= min(1.0, max(0.0, (cosA - cosT) / (cosF - cosT)));
        vec3  radiance = spotLightIntensity[i] * attenuation;
        // Diffuse.
        diffuse = Diffuse(Diffuse_color, radiance, Normal, vsLightDir);
        // Specular.
        specular = Specular(Specular_color, radiance, FragPos, Normal, vsLightDir, 16.0); // Hard-coded shininess.
        spotLight += diffuse + specular;
    }

    lighting += dirLight + pointLight + spotLight;
    FragColor = vec4(lighting, 1.0);
}