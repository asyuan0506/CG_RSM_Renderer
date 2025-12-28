#ifndef SKYBOX_H
#define SKYBOX_H

#include "headers.h"
#include "imagetexture.h"
#include "shaderprog.h"
#include "material.h"
#include "camera.h"


// VertexPT Declarations.
struct VertexPT
{
	VertexPT() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPT(glm::vec3 p, glm::vec2 uv) {
		position = p;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
};


// Skybox Declarations.
class Skybox
{
public:
	// Skybox Public Methods.
	Skybox(const std::string& texImagePath, const int nSlices, 
			const int nStacks, const float radius);
	~Skybox();
	void Render(Camera* camera, SkyboxShaderProg* shader);
	
	void SetRotation(const float newRotation) { rotationY = newRotation; }
	void SetRotateMatrix(const glm::mat4x4& rotMat) { rotate_matrix_ = rotMat; }

	ImageTexture* GetTexture() { return panorama; };
	float GetRotation() const  { return rotationY; }
	glm::mat4x4 GetRotateMatrix() const { return rotate_matrix_; }

private:
	// Skybox Private Methods.
	static void CreateSphere3D(const int nSlices, const int nStacks, const float radius, 
					std::vector<VertexPT>& vertices, std::vector<unsigned int>& indices);

	// Skybox Private Data.
	GLuint vboId;
	GLuint iboId;
	std::vector<VertexPT> vertices;
	std::vector<unsigned int> indices;
	
	SkyboxMaterial* material;
	ImageTexture* panorama;

	float rotationY; // Not used
	glm::mat4x4 rotate_matrix_;
};

#endif

