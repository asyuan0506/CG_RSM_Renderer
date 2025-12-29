#include "shaderprog.h"

#define MAX_BUFFER_SIZE 1024

ShaderProg::ShaderProg()
{
    // Create OpenGL shader program.
    shaderProgId = glCreateProgram();
    if (shaderProgId == 0) {
        std::cerr << "[ERROR] Failed to create shader program" << std::endl;
        exit(1);
    }
    // locM = locV = locP = -1;
    locMVP = -1;
}

ShaderProg::~ShaderProg()
{
    glDeleteProgram(shaderProgId);
}

bool ShaderProg::LoadFromFiles(const std::string vsFilePath, const std::string fsFilePath)
{
    // Load the vertex shader from a source file and attach it to the shader program.
    std::string vs, fs;
    if (!LoadShaderTextFromFile(vsFilePath, vs)) {
        std::cerr << "[ERROR] Failed to load vertex shader source: " << vsFilePath << std::endl;
        return false;
    }
    GLuint vsId = AddShader(vs, GL_VERTEX_SHADER);

    // Load the fragment shader from a source file and attach it to the shader program.
    if (!LoadShaderTextFromFile(fsFilePath, fs)) {
        std::cerr << "[ERROR] Failed to load vertex shader source: " << fsFilePath << std::endl;
        return false;
    };
    GLuint fsId = AddShader(fs, GL_FRAGMENT_SHADER);

    // Link and compile shader programs.
    GLint success = 0;
    GLchar errorLog[MAX_BUFFER_SIZE] = { 0 };
    glLinkProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        std::cerr << "[ERROR] Failed to link shader program: " <<  errorLog << std::endl;
        return false;
    }

    // Now the program already has all stage information, we can delete the shaders now.
    glDeleteShader(vsId);
    glDeleteShader(fsId);

    // Validate program.
    glValidateProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgId, sizeof(errorLog), NULL, errorLog);
        std::cerr << "[ERROR] Invalid shader program: " << errorLog << std::endl;
        return false;
    }

    // Update the location of uniform variables.
    GetUniformVariableLocation();

    return true;
}

void ShaderProg::GetUniformVariableLocation()
{
    locMVP = glGetUniformLocation(shaderProgId, "MVP");
}

GLuint ShaderProg::AddShader(const std::string& sourceText, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);
    if (shaderObj == 0) {
        std::cerr << "[ERROR] Failed to create shader with type " << shaderType << std::endl;
        exit(0);
    }

    const GLchar* p[1];
    p[0] = sourceText.c_str();
    GLint lengths[1];
    lengths[0] = (GLint)(sourceText.length());
    glShaderSource(shaderObj, 1, p, lengths);
    glCompileShader(shaderObj);

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[MAX_BUFFER_SIZE];
        glGetShaderInfoLog(shaderObj, MAX_BUFFER_SIZE, NULL, infoLog);
        std::cerr << "[ERROR] Failed to compile shader with type: " << shaderType << ". Info: " << infoLog << std::endl;
        exit(1);
    }

    glAttachShader(shaderProgId, shaderObj);

    return shaderObj;
}

bool ShaderProg::LoadShaderTextFromFile(const std::string filePath, std::string& sourceText)
{
    std::ifstream sourceFile(filePath.c_str());
    if(!sourceFile) {
        std::cerr << "[ERROR] Failed to open shader source file: " << filePath << std::endl;
        return false;
    }
    sourceText.assign((std::istreambuf_iterator< char >(sourceFile)),  std::istreambuf_iterator< char >());
    return true;
}

// ------------------------------------------------------------------------------------------------

FillColorShaderProg::FillColorShaderProg()
{
    locFillColor = -1;
}

FillColorShaderProg::~FillColorShaderProg()
{}

void FillColorShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locFillColor = glGetUniformLocation(shaderProgId, "fillColor");
}

// ------------------------------------------------------------------------------------------------

PhongShadingDemoShaderProg::PhongShadingDemoShaderProg()
{
    locM = -1;
    locV_ = -1;
    locNM = -1;
    locCameraPos = -1;
    locKa = -1;
    locKd = -1;
    locKs = -1;
    locNs = -1;
    locAmbientLight = -1;
    locDirLightDir = -1;
	locDirLightRadiance = -1;
	locPointLightPos = -1;
	locPointLightIntensity = -1;
    // -------------------------------------------------------
	// Add your code for initializing the data of spot light.
    locSpotLightPos_ = -1;
    locSpotLightIntensity_ = -1;
	locSpotLightDir_ = -1;
    locSpotLightTotalWidth_ = -1;
    locSpotLightCutoffStart_ = -1;
	// -------------------------------------------------------
    // -------------------------------------------------------
	// Add your code for initializing the data of textures.
	locMapKd = -1;
	// -------------------------------------------------------
}

PhongShadingDemoShaderProg::~PhongShadingDemoShaderProg()
{}

void PhongShadingDemoShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locM = glGetUniformLocation(shaderProgId, "worldMatrix");
	locV_ = glGetUniformLocation(shaderProgId, "viewMatrix");
    locNM = glGetUniformLocation(shaderProgId, "normalMatrix");
    locCameraPos = glGetUniformLocation(shaderProgId, "cameraPos");
    locKa = glGetUniformLocation(shaderProgId, "Ka");
    locKd = glGetUniformLocation(shaderProgId, "Kd");
    locKs = glGetUniformLocation(shaderProgId, "Ks");
    locNs = glGetUniformLocation(shaderProgId, "Ns");
    locAmbientLight = glGetUniformLocation(shaderProgId, "ambientLight");
    locDirLightDir = glGetUniformLocation(shaderProgId, "dirLightDir");
	locDirLightRadiance = glGetUniformLocation(shaderProgId, "dirLightRadiance");
	locPointLightPos = glGetUniformLocation(shaderProgId, "pointLightPos");
	locPointLightIntensity = glGetUniformLocation(shaderProgId, "pointLightIntensity");
    // -------------------------------------------------------
	// Add your code for getting the location of data of spot light.
	locSpotLightPos_ = glGetUniformLocation(shaderProgId, "spotLightPosition");
	locSpotLightIntensity_ = glGetUniformLocation(shaderProgId, "spotLightIntensity");
	locSpotLightDir_ = glGetUniformLocation(shaderProgId, "spotLightDirection");
	locSpotLightTotalWidth_ = glGetUniformLocation(shaderProgId, "spotLightTotalWidthInDegree");
	locSpotLightCutoffStart_ = glGetUniformLocation(shaderProgId, "spotLightCutoffStartInDegree");
	// -------------------------------------------------------
    // -------------------------------------------------------
	// Add your code for getting the location of texture variable.
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
	// -------------------------------------------------------

}

// ------------------------------------------------------------------------------------------------

SkyboxShaderProg::SkyboxShaderProg()
{
    locMapKd = -1;
}

SkyboxShaderProg::~SkyboxShaderProg()
{}

void SkyboxShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locMapKd = glGetUniformLocation(shaderProgId, "mapKd");
}

// ------------------------------------------------------------------------------------------------
GBufferShaderProg::GBufferShaderProg()
{
	locM_ = -1;
	locV_ = -1;
	locP_ = -1;
    locNM_ = -1;
	locTexDiffuse_ = -1;
	locKs_ = -1;
}

void GBufferShaderProg::GetUniformVariableLocation() {
    ShaderProg::GetUniformVariableLocation();
    locM_ = glGetUniformLocation(shaderProgId, "worldMatrix");
    locV_ = glGetUniformLocation(shaderProgId, "viewMatrix");
    locP_ = glGetUniformLocation(shaderProgId, "projectionMatrix");
	locNM_ = glGetUniformLocation(shaderProgId, "normalMatrix");
	locTexDiffuse_ = glGetUniformLocation(shaderProgId, "texture_diffuse1");
	locKs_ = glGetUniformLocation(shaderProgId, "Ks");
}

GBufferShaderProg::~GBufferShaderProg()
{}
// ------------------------------------------------------------------------------------------------
DeferredShadingShaderProg::DeferredShadingShaderProg()
{
    locGPosition_ = -1;
    locGNormal_ = -1;
    locGAlbedoSpec_ = -1;
    locV_ = -1;
    // Light data.
    locDirLightDir = -1;
    locDirLightRadiance = -1;
    locPointLightPos = -1;
    locPointLightIntensity = -1;
    locSpotLightPos_ = -1;
    locSpotLightIntensity_ = -1;
    locSpotLightDir_ = -1;
    locSpotLightCutoffStart_ = -1;
    locSpotLightTotalWidth_ = -1;
}
DeferredShadingShaderProg::~DeferredShadingShaderProg()
{}

void DeferredShadingShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locGPosition_ = glGetUniformLocation(shaderProgId, "gPosition");
    locGNormal_ = glGetUniformLocation(shaderProgId, "gNormal");
    locGAlbedoSpec_ = glGetUniformLocation(shaderProgId, "gAlbedoSpec");
    locV_ = glGetUniformLocation(shaderProgId, "viewMatrix");
    // Light data.
    locDirLightDir = glGetUniformLocation(shaderProgId, "dirLightDir");
    locDirLightRadiance = glGetUniformLocation(shaderProgId, "dirLightRadiance");
    locPointLightPos = glGetUniformLocation(shaderProgId, "pointLightPos");
    locPointLightIntensity = glGetUniformLocation(shaderProgId, "pointLightIntensity");
    locSpotLightPos_ = glGetUniformLocation(shaderProgId, "spotLightPosition");
    locSpotLightIntensity_ = glGetUniformLocation(shaderProgId, "spotLightIntensity");
    locSpotLightDir_ = glGetUniformLocation(shaderProgId, "spotLightDirection");
    locSpotLightCutoffStart_ = glGetUniformLocation(shaderProgId, "spotLightCutoffStartInDegree");
    locSpotLightTotalWidth_ = glGetUniformLocation(shaderProgId, "spotLightTotalWidthInDegree");
}
// ------------------------------------------------------------------------------------------------
LightPassShaderProg::LightPassShaderProg()
{}

LightPassShaderProg::~LightPassShaderProg()
{}

void LightPassShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
}
// ------------------------------------------------------------------------------------------------
RSMBufferShaderProg::RSMBufferShaderProg()
{
    locM_ = -1;
    locV_ = -1;
    locLightVP_ = -1;
	locLightIntensity_ = -1;
    locNM_ = -1;
    locTexDiffuse_ = -1;
    locKs_ = -1;
}
RSMBufferShaderProg::~RSMBufferShaderProg()
{
}
void RSMBufferShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
    locM_ = glGetUniformLocation(shaderProgId, "worldMatrix");
    locV_ = glGetUniformLocation(shaderProgId, "viewMatrix");
    locNM_ = glGetUniformLocation(shaderProgId, "normalMatrix");
	locLightVP_ = glGetUniformLocation(shaderProgId, "lightViewVP");
	locLightIntensity_ = glGetUniformLocation(shaderProgId, "lightIntensity");
    locTexDiffuse_ = glGetUniformLocation(shaderProgId, "texture_diffuse");
    locKs_ = glGetUniformLocation(shaderProgId, "Ks");
}

RSMShadingShaderProg::RSMShadingShaderProg()
{
    locV_ = -1;
    locP_ = -1;
    locAlbedoTexture_ = -1;
    locNormalTexture_ = -1;
    locPositionTexture_ = -1;
    locRSMFluxTexture_ = -1;
    locRSMNormalTexture_ = -1;
    locRSMPositionTexture_ = -1;
    locLightVPMatrixMulInverseCameraViewMatrix_ = -1;
    locMaxSampleRadius_ = -1;
    locRSMSize_ = -1;
    locVPLNum_ = -1;

	locLightNum_ = -1;
	locLightType_ = -1;
	locLightIntensity_ = -1;
	locLightPosInViewSpace_ = -1;
	locLightDirInViewSpace_ = -1;
	locSpotCutoff_ = -1;
	locSpotTotalWidth_ = -1;
}
RSMShadingShaderProg::~RSMShadingShaderProg()
{
}

void RSMShadingShaderProg::GetUniformVariableLocation()
{
    ShaderProg::GetUniformVariableLocation();
	locV_ = glGetUniformLocation(shaderProgId, "ViewMatrix");
	locP_ = glGetUniformLocation(shaderProgId, "ProjectionMatrix");
	locAlbedoTexture_ = glGetUniformLocation(shaderProgId, "u_AlbedoTexture");
	locNormalTexture_ = glGetUniformLocation(shaderProgId, "u_NormalTexture");
	locPositionTexture_ = glGetUniformLocation(shaderProgId, "u_PositionTexture");
	locRSMFluxTexture_ = glGetUniformLocation(shaderProgId, "u_RSMFluxTexture");
	locRSMNormalTexture_ = glGetUniformLocation(shaderProgId, "u_RSMNormalTexture");
	locRSMPositionTexture_ = glGetUniformLocation(shaderProgId, "u_RSMPositionTexture");
	locLightVPMatrixMulInverseCameraViewMatrix_ = glGetUniformLocation(shaderProgId, "u_LightVPMatrixMulInverseCameraViewMatrix");
	locMaxSampleRadius_ = glGetUniformLocation(shaderProgId, "u_MaxSampleRadius");
	locRSMSize_ = glGetUniformLocation(shaderProgId, "u_RSMSize");
	locVPLNum_ = glGetUniformLocation(shaderProgId, "u_VPLNum");

	// Light properties.
	locLightNum_ = glGetUniformLocation(shaderProgId, "u_LightNum");
	locLightType_ = glGetUniformLocation(shaderProgId, "u_LightType");
	locLightIntensity_ = glGetUniformLocation(shaderProgId, "u_LightIntensity");
	locLightPosInViewSpace_ = glGetUniformLocation(shaderProgId, "u_LightPosInViewSpace");
	locLightDirInViewSpace_ = glGetUniformLocation(shaderProgId, "u_LightDirInViewSpace");
	locSpotCutoff_ = glGetUniformLocation(shaderProgId, "u_SpotCosCutoff");
	locSpotTotalWidth_ = glGetUniformLocation(shaderProgId, "u_SpotCosTotalWidth");

}