#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "headers.h"
#include "material.h"

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

// SubMesh Declarations.
struct SubMesh
{
	SubMesh() {
		material = nullptr;
		iboId = 0;
	}
	PhongMaterial* material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};


// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();

	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath, const bool normalized = true);

	// Show model information.
	void ShowInfo();

	// -------------------------------------------------------
	// Feel free to add your methods or data here.
	bool LoadMTLFromFile(const std::string& mtlFilePath);
	void LoadVertexIndices(std::stringstream& ss, SubMesh* sub_mesh,
		std::unordered_map<std::string, unsigned int>& unique_vertices,
		std::vector<glm::vec3>& positions,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec2>& uvs);
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void Render(SubMesh& sub_mesh);
	// Getter
	std::vector<SubMesh> GetSubMeshes() { return subMeshes; }
	int GetModelID() const { return model_id_; }
	glm::vec3 GetScaleVector() const { return scale_vector_; }
	glm::mat4x4 GetRotateMatrix() const { return rotate_matrix_; }
	glm::vec3 GetTranslateVector() const { return translate_vector_; }
	glm::mat4x4 GetWorldMatrix() const { return world_matrix_; } 
	// Setter
	void SetScaleVector(const glm::vec3& sv) { scale_vector_ = sv; }
	void SetRotateMatrix(const glm::mat4x4& rm) { rotate_matrix_ = rm; }
	void SetTranslateVector(const glm::vec3& tv) { translate_vector_ = tv; }
	void SetModelID(const int id) { model_id_ = id; }
	void UpdateWorldMatrix(); // Must be called after transformation
	// -------------------------------------------------------

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumSubMeshes() const { return (int)subMeshes.size(); }

	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

private:
	// -------------------------------------------------------
	// Feel free to add your methods or data here.
	int model_id_;
	glm::vec3 scale_vector_;
	glm::mat4x4 rotate_matrix_;
	glm::vec3 translate_vector_;
	glm::mat4x4 world_matrix_;
	std::vector<PhongMaterial> materials_;
	std::vector<ImageTexture*> textures_; // Handle multiple textures address
	// -------------------------------------------------------

	// TriangleMesh Private Data.
	GLuint vboId;
	
	std::vector<VertexPTN> vertices;
	// For supporting multiple materials per object, move to SubMesh.
	// GLuint iboId;
	// std::vector<unsigned int> vertexIndices;
	std::vector<SubMesh> subMeshes;

	int numVertices;
	int numTriangles;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
};


#endif
