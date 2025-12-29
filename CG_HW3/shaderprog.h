#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "headers.h"

// ShaderProg Declarations.
class ShaderProg
{
public:
	// ShaderProg Public Methods.
	ShaderProg();
	~ShaderProg();

	bool LoadFromFiles(const std::string vsFilePath, const std::string fsFilePath);
	void Bind() { glUseProgram(shaderProgId); };
	void UnBind() { glUseProgram(0); };

	GLint GetLocMVP() const { return locMVP; }

protected:
	// ShaderProg Protected Methods.
	virtual void GetUniformVariableLocation();

	// ShaderProg Protected Data.
	GLuint shaderProgId;

private:
	// ShaderProg Private Methods.
	GLuint AddShader(const std::string& sourceText, GLenum shaderType);
	static bool LoadShaderTextFromFile(const std::string filePath, std::string& sourceText);

	// ShaderProg Private Data.
	GLint locMVP;
};

// ------------------------------------------------------------------------------------------------

// FillColorShaderProg Declarations.
class FillColorShaderProg : public ShaderProg
{
public:
	// FillColorShaderProg Public Methods.
	FillColorShaderProg();
	~FillColorShaderProg();

	GLint GetLocFillColor() const { return locFillColor; }

protected:
	// FillColorShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// FillColorShaderProg Private Data.
	GLint locFillColor;
};

// ------------------------------------------------------------------------------------------------

// PhongShadingDemoShaderProg Declarations.
class PhongShadingDemoShaderProg : public ShaderProg
{
public:
	// PhongShadingDemoShaderProg Public Methods.
	PhongShadingDemoShaderProg();
	~PhongShadingDemoShaderProg();

	GLint GetLocM() const { return locM; }
	GLint GetLocV() const { return locV_; }
	GLint GetLocNM() const { return locNM; }
	GLint GetLocCameraPos() const { return locCameraPos; }
	GLint GetLocKa() const { return locKa; }
	GLint GetLocKd() const { return locKd; }
	GLint GetLocKs() const { return locKs; }
	GLint GetLocNs() const { return locNs; }
	GLint GetLocAmbientLight() const { return locAmbientLight; }
	GLint GetLocDirLightDir() const { return locDirLightDir; }
	GLint GetLocDirLightRadiance() const { return locDirLightRadiance; }
	GLint GetLocPointLightPos() const { return locPointLightPos; }
	GLint GetLocPointLightIntensity() const { return locPointLightIntensity; }
	// -------------------------------------------------------
	// Add your methods for spot light.
	GLint GetLocSpotLightPos() const { return locSpotLightPos_; }
	GLint GetLocSpotLightIntensity() const { return locSpotLightIntensity_; }
	GLint GetLocSpotLightDir() const { return locSpotLightDir_; }
	GLint GetLocSpotLightCutoffStart() const { return locSpotLightCutoffStart_; }
	GLint GetLocSpotLightTotalWidth() const { return locSpotLightTotalWidth_; }
	// -------------------------------------------------------
	// -------------------------------------------------------
	// Add your methods for supporting textures.
	GLint GetLocMapKd() const { return locMapKd; }
	// -------------------------------------------------------

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// PhongShadingDemoShaderProg Public Data.
	// Transformation matrix.
	GLint locM;
	GLint locV_;
	GLint locNM;
	GLint locCameraPos;
	// Material properties.
	GLint locKa;
	GLint locKd;
	GLint locKs;
	GLint locNs;
	// Light data.
	GLint locAmbientLight;
	GLint locDirLightDir;
	GLint locDirLightRadiance;
	GLint locPointLightPos;
	GLint locPointLightIntensity;
	// -------------------------------------------------------
	// Add your data for spot light.
	GLint locSpotLightPos_;
	GLint locSpotLightIntensity_;
	GLint locSpotLightDir_;
	GLint locSpotLightCutoffStart_;
	GLint locSpotLightTotalWidth_;
	// -------------------------------------------------------
	// Texture data.
	// -------------------------------------------------------
	// Add your data for supporting textures.
	GLint locMapKd;
	// -------------------------------------------------------
};

// ------------------------------------------------------------------------------------------------

// SkyboxShaderProg Declarations.
class SkyboxShaderProg : public ShaderProg
{
public:
	// SkyboxShaderProg Public Methods.
	SkyboxShaderProg();
	~SkyboxShaderProg();

	GLint GetLocMapKd() const { return locMapKd; }

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// SkyboxShaderProg Public Data.
	GLint locMapKd;
};

class GBufferShaderProg : public ShaderProg
{
public:
	// GBufferShaderProg Public Methods.
	GBufferShaderProg();
	~GBufferShaderProg();
	GLint GetLocM() const { return locM_; }
	GLint GetLocV() const { return locV_; }
	GLint GetLocP() const { return locP_; }
	GLint GetLocNM() const { return locNM_; }
	GLint GetLocTexDiffuse() const { return locTexDiffuse_; }
	GLint GetLocKs() const { return locKs_; }
protected:
	// GBufferShaderProg Protected Methods.
	void GetUniformVariableLocation();
private:
	// GBufferShaderProg Private Data.
	GLint locM_;
	GLint locV_;
	GLint locP_;
	GLint locNM_;
	GLint locTexDiffuse_;
	GLint locKs_;
	
};

class DeferredShadingShaderProg : public ShaderProg
{
	public:
	// DeferredShadingShaderProg Public Methods.
	DeferredShadingShaderProg();
	~DeferredShadingShaderProg();
	GLint GetLocGPosition() const { return locGPosition_; }
	GLint GetLocGNormal() const { return locGNormal_; }
	GLint GetLocGAlbedoSpec() const { return locGAlbedoSpec_; }
	GLint GetLocV() const { return locV_; }
	// Light data.
	GLint GetLocDirLightDir() const { return locDirLightDir; }
	GLint GetLocDirLightRadiance() const { return locDirLightRadiance; }
	GLint GetLocPointLightPos() const { return locPointLightPos; }
	GLint GetLocPointLightIntensity() const { return locPointLightIntensity; }
	GLint GetLocSpotLightPos() const { return locSpotLightPos_; }
	GLint GetLocSpotLightIntensity() const { return locSpotLightIntensity_; }
	GLint GetLocSpotLightDir() const { return locSpotLightDir_; }
	GLint GetLocSpotLightCutoffStart() const { return locSpotLightCutoffStart_; }
	GLint GetLocSpotLightTotalWidth() const { return locSpotLightTotalWidth_; }
protected:
	// DeferredShadingShaderProg Protected Methods.
	void GetUniformVariableLocation();
private:
	// DeferredShadingShaderProg Private Data.
	GLint locGPosition_;
	GLint locGNormal_;
	GLint locGAlbedoSpec_;
	GLint locV_;
	// Light data.
	GLint locDirLightDir;
	GLint locDirLightRadiance;
	GLint locPointLightPos;
	GLint locPointLightIntensity;
	GLint locSpotLightPos_;
	GLint locSpotLightIntensity_;
	GLint locSpotLightDir_;
	GLint locSpotLightCutoffStart_;
	GLint locSpotLightTotalWidth_;
};

class LightPassShaderProg : public ShaderProg
{
	public:
	// LightPassShaderProg Public Methods.
	LightPassShaderProg();
	~LightPassShaderProg();
	
protected:
	// LightPassShaderProg Protected Methods.
	void GetUniformVariableLocation();
private:	
};

class RSMBufferShaderProg : public ShaderProg
{
public:
	// RSMBufferShaderProg Public Methods.
	RSMBufferShaderProg();
	~RSMBufferShaderProg();
	GLint GetLocM() const { return locM_; }
	GLint GetLocV() const { return locV_; }
	GLint GetLocLightVP() const { return locLightVP_; }
	GLint GetLocLightIntensity() const { return locLightIntensity_; }
	GLint GetLocNM() const { return locNM_; }
	GLint GetLocTexDiffuse() const { return locTexDiffuse_; }
	GLint GetLocKs() const { return locKs_; }
protected:
	// RSMBufferShaderProg Protected Methods.
	void GetUniformVariableLocation();
private:
	// RSMBufferShaderProg Private Data.
	GLint locM_;
	GLint locV_;
	GLint locLightVP_;
	GLint locLightIntensity_;
	GLint locNM_;
	GLint locTexDiffuse_;
	GLint locKs_;

};

class RSMShadingShaderProg : public ShaderProg
{
public:
	// RSMShadingShaderProg Public Methods.
	RSMShadingShaderProg();
	~RSMShadingShaderProg();
	GLint GetLocV() const { return locV_; }
	GLint GetLocP() const { return locP_; }
	GLint GetLocAlbedoTexture() const { return locAlbedoTexture_; }
	GLint GetLocNormalTexture() const { return locNormalTexture_; }
	GLint GetLocPositionTexture() const { return locPositionTexture_; }
	GLint GetLocRSMFluxTexture() const { return locRSMFluxTexture_; }
	GLint GetLocRSMNormalTexture() const { return locRSMNormalTexture_; }
	GLint GetLocRSMPositionTexture() const { return locRSMPositionTexture_; }
	GLint GetLocLightVPMatrixMulInverseCameraViewMatrix() const { return locLightVPMatrixMulInverseCameraViewMatrix_; }
	GLint GetLocMaxSampleRadius() const { return locMaxSampleRadius_; }
	GLint GetLocRSMSize() const { return locRSMSize_; }
	GLint GetLocVPLNum() const { return locVPLNum_; }

	GLint GetLocLightNum() const { return locLightNum_; }
	GLint GetLocLightType() const { return locLightType_; }
	GLint GetLocLightIntensity() const { return locLightIntensity_; }
	GLint GetLocLightPosInViewSpace() const { return locLightPosInViewSpace_; }
	GLint GetLocLightDirInViewSpace() const { return locLightDirInViewSpace_; }
	GLint GetLocSpotCutoff() const { return locSpotCutoff_; }
	GLint GetLocSpotTotalWidth() const { return locSpotTotalWidth_; }
	
protected:
	// RSMShadingShaderProg Protected Methods.
	void GetUniformVariableLocation();
private:
	// RSMShadingShaderProg Private Data.
	GLint locV_;
	GLint locP_;
	GLint locAlbedoTexture_;
	GLint locNormalTexture_;
	GLint locPositionTexture_;
	GLint locRSMFluxTexture_;
	GLint locRSMNormalTexture_;
	GLint locRSMPositionTexture_;
	GLint locLightVPMatrixMulInverseCameraViewMatrix_;
	GLint locMaxSampleRadius_;
	GLint locRSMSize_;
	GLint locVPLNum_;

	GLint locLightNum_;
	GLint locLightType_;
	GLint locLightIntensity_;
	GLint locLightPosInViewSpace_;
	GLint locLightDirInViewSpace_;
	GLint locSpotCutoff_;
	GLint locSpotTotalWidth_;
};

#endif


