#version 330 core

// Data from vertex shader.
// --------------------------------------------------------
// Add your data for interpolation.
in vec2 iTexCoord;
in vec3 vsPosition;
in vec3 vsNormal;
// --------------------------------------------------------
// --------------------------------------------------------
// Add your uniform variables.
uniform mat4 viewMatrix;
// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
uniform sampler2D mapKd;
// Light data.
const int POINT_LIGHT_CNT = 1;
const int SPOT_LIGHT_CNT = 3;
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos[POINT_LIGHT_CNT];
uniform vec3 pointLightIntensity[POINT_LIGHT_CNT];
uniform vec3 spotLightPosition[SPOT_LIGHT_CNT];
uniform vec3 spotLightDirection[SPOT_LIGHT_CNT];
uniform vec3 spotLightIntensity[SPOT_LIGHT_CNT];
uniform float spotLightCutoffStartInDegree[SPOT_LIGHT_CNT];
uniform float spotLightTotalWidthInDegree[SPOT_LIGHT_CNT];
// --------------------------------------------------------

out vec4 FragColor;

vec3 ReflectDir(vec3 lightDir, vec3 N) { // Not used
    lightDir = -lightDir; // Change to original light direction.
    return lightDir - 2.0 * dot(N, lightDir) * N;
}

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 P, vec3 N, vec3 lightDir, float Ns)
{
    // Try to implement yourself!
    vec3 viewDir = normalize(-P); // View direction in view space.
    vec3 VH = normalize(lightDir + viewDir); // View half vector.
    return Ks * I * pow(max(0, dot(N, VH)), Ns);
}


void main()
{
    // ----------------------------------------------------------------------
    // Modify the code below to implement Phong shading with texture support.
    // ----------------------------------------------------------------------
    vec3 texColor = texture2D(mapKd, iTexCoord).rgb;
    vec3 N = normalize(vsNormal);
    // -------------------------------------------------------------
    // Ambient light.
    vec3 ambient = Ka * ambientLight;
    // -------------------------------------------------------------
    // Directional light.
    vec3 vsLightDir = (viewMatrix * vec4(-dirLightDir, 0.0)).xyz;
    vsLightDir = normalize(vsLightDir);
    // Diffuse.
    vec3 diffuse = Diffuse(texColor, dirLightRadiance, N, vsLightDir);
    // Specular.
    vec3 specular = Specular(Ks, dirLightRadiance, vsPosition, N, vsLightDir, Ns);
    vec3 dirLight = diffuse + specular;
    // -------------------------------------------------------------
    // Point light.
    vec3 pointLight = vec3(0.0);
    for(int i = 0; i < POINT_LIGHT_CNT; i++) {
        vec4  tmpPos = viewMatrix * vec4(pointLightPos[i], 1.0);
        vec3  vsLightPos = tmpPos.xyz / tmpPos.w;
        vsLightDir = normalize(vsLightPos - vsPosition);
        float distSurfaceToLight = distance(vsLightPos, vsPosition);
        float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
        vec3 radiance = pointLightIntensity[i] * attenuation;
        // Diffuse.
        diffuse = Diffuse(texColor, radiance, N, vsLightDir);
        // Specular.
        specular = Specular(Ks, radiance, vsPosition, N, vsLightDir, Ns);
        pointLight += diffuse + specular;
    }
    // -------------------------------------------------------------
    // Spot light.
    vec3 spotLight = vec3(0.0);
    for (int i = 0; i < SPOT_LIGHT_CNT; i++) {
        vec4 tmpPos = viewMatrix * vec4(spotLightPosition[i], 1.0);
        vec3 vsLightPos = tmpPos.xyz / tmpPos.w;
        vsLightDir = normalize(vsLightPos - vsPosition);  // Light direction from surface to light.
        vec3 vsSpotDir = (viewMatrix * vec4(-spotLightDirection[i], 0.0)).xyz; // Reverse the direction.
        vsSpotDir = normalize(vsSpotDir);

        float distSurfaceToLight = distance(vsLightPos, vsPosition);
        float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);

        float cosA = dot(vsSpotDir, vsLightDir);  // A, T, F explained in the PPT.
        float cosF = cos(radians(spotLightCutoffStartInDegree[i]));
        float cosT = cos(radians(spotLightTotalWidthInDegree[i]));

              attenuation *= min(1.0, max(0.0, (cosA - cosT) / (cosF - cosT)));
        vec3  radiance = spotLightIntensity[i] * attenuation;
        // Diffuse.
        diffuse = Diffuse(texColor, radiance, N, vsLightDir);
        // Specular.
        specular = Specular(Ks, radiance, vsPosition, N, vsLightDir, Ns);
        spotLight = spotLight + diffuse + specular;
    }
    // -------------------------------------------------------------
    vec3 LightingColor = ambient + dirLight + pointLight + spotLight;
    FragColor = vec4(LightingColor, 1);
}
