#version 330 core
out vec4 FragColor;

in vec2 iTexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D u_PositionTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_RSMPositionTexture;	
uniform sampler2D u_RSMNormalTexture;
uniform sampler2D u_RSMFluxTexture;
	
uniform mat4  u_LightVPMatrixMulInverseCameraViewMatrix;
uniform float u_MaxSampleRadius;
uniform int   u_RSMSize;
uniform int   u_VPLNum;
// Light data.
uniform int   u_LightNum; // NOT YET USED
uniform int   u_LightType;           // 0: Directional, 1: Spot Light
uniform vec3  u_LightIntensity;   
uniform vec3  u_LightPosInViewSpace; // Spot Light
uniform vec3  u_LightDirInViewSpace; // Directional & Spot Light 
uniform float u_SpotCosCutoff;       // Spot Light, in degree
uniform float u_SpotCosTotalWidth;   // Spot Light, in degree


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 P, vec3 N, vec3 lightDir, float Ns)
{
    vec3 viewDir = normalize(-P); // View direction in view space.
    vec3 VH = normalize(lightDir + viewDir); // View half vector.
    return Ks * I * pow(max(0, dot(N, VH)), Ns);
}


vec3 calcVPLIrradiance(vec3 vVPLFlux, vec3 vVPLNormal, vec3 vVPLPos, vec3 vFragPos, vec3 vFragNormal, float vWeight)
{
	vec3 VPL2Frag = normalize(vFragPos - vVPLPos);

	return vVPLFlux * max(dot(vVPLNormal, VPL2Frag), 0) * max(dot(vFragNormal, -VPL2Frag), 0) * vWeight;
}

void main()
{    
	vec3 FragViewNormal = normalize(texture(u_NormalTexture, iTexCoord).xyz);
	vec3 FragAlbedo = texture(u_AlbedoTexture, iTexCoord).rgb;  
    vec3 FragKs = vec3(texture(u_AlbedoTexture, iTexCoord).a);
	vec3 FragViewPos = texture(u_PositionTexture, iTexCoord).xyz; // view space position

	vec4 FragPosInLightSpace = u_LightVPMatrixMulInverseCameraViewMatrix * vec4(FragViewPos, 1);
	FragPosInLightSpace /= FragPosInLightSpace.w;
	vec2 FragNDCPos4Light = (FragPosInLightSpace.xy + 1) / 2;
	float RSMTexelSize = 1.0f / u_RSMSize;

    vec3 DirectIllumination = vec3(0.0f);

	bool outLightSpace = FragPosInLightSpace.z < -1.0f || FragPosInLightSpace.x > 1.0f || FragPosInLightSpace.y > 1.0f || FragPosInLightSpace.x < -1.0f || FragPosInLightSpace.y < -1.0f;
    vec3 RSMPos = texture(u_RSMPositionTexture, FragNDCPos4Light).xyz;
    bool inShadow = length(FragViewPos - RSMPos) > 0.1f; // Bias 0.1, because RSM pos is interpolated

    if (outLightSpace || inShadow)
    {
        DirectIllumination = vec3(0.1) * FragAlbedo / u_LightNum; // Ambient term
    }
    else {
	    vec3 lightDir;
        float attenuation = 1.0f;

        if (u_LightType == 0) // Directional Light
        {
            lightDir = normalize(-u_LightDirInViewSpace);
        }
        else // Local Light
        {
            vec3 lightVec = u_LightPosInViewSpace - FragViewPos;
            lightDir = normalize(lightVec);
            float dist = length(lightDir);
            
            attenuation = 1.0f / (dist * dist);
        }

        // Spot Light
        if (u_LightType == 1) 
        {
            float cosA = dot(-normalize(u_LightDirInViewSpace), lightDir);  // A, T, F explained in the PPT.
            float cosF = cos(radians(u_SpotCosCutoff));
            float cosT = cos(radians(u_SpotCosTotalWidth));
            attenuation *= min(1.0f, max(0.0f, (cosA - cosT) / (cosF - cosT)));
        }

        DirectIllumination = attenuation * (Diffuse(FragAlbedo, u_LightIntensity, FragViewNormal, lightDir) + 
                                            Specular(FragKs, u_LightIntensity, FragViewPos, FragViewNormal, lightDir, 250.0f)); // Ns = 250.0f
        
    }
    // ---------------------------

	vec3 IndirectIllumination = vec3(0.0f);
	float goldenAngle = 2.39996323f; 

	for(int i = 0; i < u_VPLNum; ++i)
	{
        float r = sqrt(float(i) + 0.5f) / sqrt(float(u_VPLNum));
        float theta = i * goldenAngle;
        vec2 offset = vec2(cos(theta), sin(theta)) * r;
        float weight = 2.0f; 

		vec2 VPLSamplePos = FragNDCPos4Light + u_MaxSampleRadius * offset * RSMTexelSize;
		vec3 VPLFlux = texture(u_RSMFluxTexture, VPLSamplePos).xyz;
		vec3 VPLNormalInViewSpace = normalize(texture(u_RSMNormalTexture, VPLSamplePos).xyz);
		vec3 VPLPositionInViewSpace = texture(u_RSMPositionTexture, VPLSamplePos).xyz;

		IndirectIllumination += calcVPLIrradiance(VPLFlux, VPLNormalInViewSpace, VPLPositionInViewSpace, FragViewPos, FragViewNormal, weight);
	}
	IndirectIllumination *= FragAlbedo;

	vec3 Result = DirectIllumination + IndirectIllumination / u_VPLNum;

	FragColor = vec4(Result, 1.0f);
}