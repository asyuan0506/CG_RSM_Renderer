#include "trianglemesh.h"


// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	// -------------------------------------------------------
	// Add your initialization code here.
	scale_vector_ = glm::vec3(1.5f, 1.5f, 1.5f);
	rotate_matrix_ = glm::mat4x4(1.0f);
	// Test rotation matrix (30 degrees around Y axis)
	//rotate_matrix_[0][0] = 0.866f; rotate_matrix_[0][2] = -0.5f; rotate_matrix_[2][0] = 0.5f; rotate_matrix_[2][2] = 0.866f;
	translate_vector_ = glm::vec3(0.0f, 0.0f, 0.0f);
	UpdateWorldMatrix();
	model_id_ = -1;
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	vboId = 0;
	// -------------------------------------------------------
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	// -------------------------------------------------------
	// Add your release code here.
	vertices.clear();

	for (int i = 0; i < textures_.size(); i++) {
		delete textures_[i];
	}

	materials_.clear();
	for (SubMesh& sub_mesh : subMeshes) {
		sub_mesh.vertexIndices.clear();
		glDeleteBuffers(1, &sub_mesh.iboId);
	}
	subMeshes.clear();

	glDeleteBuffers(1, &vboId);
	// -------------------------------------------------------
}

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{
	// Parse the OBJ file.
	// ---------------------------------------------------------------------------
	// Add your implementation here (HW1 + read *.MTL).
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filePath << std::endl;
		return false;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::unordered_map<std::string, unsigned int> unique_vertices;
	SubMesh* current_sub_mesh_ptr = nullptr;

	std::stringstream ss;
	std::string line;
	float max_x = -FLT_MAX, max_y = -FLT_MAX, max_z = -FLT_MAX;
	float min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
	while (std::getline(file, line)) {
		ss << line;
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			glm::vec3 position(x, y, z);
			max_x = std::max(max_x, x); min_x = std::min(min_x, x);
			max_y = std::max(max_y, y); min_y = std::min(min_y, y);
			max_z = std::max(max_z, z); min_z = std::min(min_z, z);
			positions.push_back(position);
		}
		else if (prefix == "vt") {
			float x, y;
			ss >> x >> y;
			glm::vec2 texcoord(x, y);
			uvs.push_back(texcoord);
		}
		else if (prefix == "vn") {
			float x, y, z;
			ss >> x >> y >> z;
			glm::vec3 normal(x, y, z);
			normals.push_back(normal);
		}
		else if (prefix == "f") {
			LoadVertexIndices(ss, current_sub_mesh_ptr, unique_vertices, positions, normals, uvs); // Check current_sub_mesh
		}
		else if (prefix == "mtllib") { // Read MTL file and store materials.
			std::string mtl_file_name;
			ss >> mtl_file_name;
			std::string mtl_file_path = filePath.substr(0, filePath.find_last_of("\\")) +
				"\\" + mtl_file_name;
			LoadMTLFromFile(mtl_file_path);
		}
		else if (prefix == "usemtl") {
			SubMesh sub_mesh;
			std::string material_name;
			ss >> material_name;
			for (int i = 0; i < materials_.size(); i++) {
				PhongMaterial* mtl_ptr = &materials_[i];
				if (mtl_ptr->GetName() == material_name) {
					sub_mesh.material = mtl_ptr;
				}
			}
			subMeshes.push_back(sub_mesh);
			current_sub_mesh_ptr = &subMeshes.back();
		}
		ss.clear();
		ss.str(std::string());
	}
	// ---------------------------------------------------------------------------

	// Normalize the geometry data.
	if (normalized) {
		// -----------------------------------------------------------------------
		// Add your normalization code here (HW1).
		float scale = std::max({ max_x - min_x, max_y - min_y, max_z - min_z });
		objCenter = glm::vec3((max_x + min_x) / 2.0f, (max_y + min_y) / 2.0f, (max_z + min_z) / 2.0f);
		for (int i = 0; i < numVertices; i++) {
			vertices[i].position = (vertices[i].position - objCenter) / scale;
		}
		// -----------------------------------------------------------------------
	}
	return true;
}

bool TriangleMesh::LoadMTLFromFile(const std::string& mtlFilePath)
{
	std::ifstream mtl_file(mtlFilePath);
	if (!mtl_file.is_open()) {
		std::cerr << "Failed to open MTL file: " << mtlFilePath << std::endl;
		return false;
	}

	std::string line;
	std::stringstream ss;
	PhongMaterial* material = nullptr;

	while (std::getline(mtl_file, line)) {
		ss << line;
		std::string prefix;
		ss >> prefix;

		if (prefix == "newmtl") {
			std::string material_name;
			ss >> material_name;
			PhongMaterial tmp_material;
			tmp_material.SetName(material_name);
			materials_.push_back(tmp_material);
			material = &materials_.back();
		}
		else if (prefix == "Ns") {
			float ns;
			ss >> ns;
			material->SetNs(ns);
		}
		else if (prefix == "Ka") {
			float ka_r, ka_g, ka_b;
			ss >> ka_r >> ka_g >> ka_b;
			material->SetKa(glm::vec3(ka_r, ka_g, ka_b));
		}
		else if (prefix == "Kd") {
			float kd_r, kd_g, kd_b;
			ss >> kd_r >> kd_g >> kd_b;
			material->SetKd(glm::vec3(kd_r, kd_g, kd_b));
			ImageTexture* default_texture = new ImageTexture(kd_r, kd_g, kd_b); // Need to make sure map_Kd read after Kd
			textures_.push_back(default_texture);
			material->SetMapKd(default_texture);
		}
		else if (prefix == "Ks") {
			float ks_r, ks_g, ks_b;
			ss >> ks_r >> ks_g >> ks_b;
			material->SetKs(glm::vec3(ks_r, ks_g, ks_b));
		}
		else if (prefix == "map_Kd") {
			delete textures_.back();
			textures_.pop_back(); // Remove the default texture created by Kd

			std::string texture_file_path;
			ss >> texture_file_path;
			texture_file_path = mtlFilePath.substr(0, mtlFilePath.find_last_of("\\")) +
				"\\" + texture_file_path;
			ImageTexture* texture = new ImageTexture(texture_file_path);
			textures_.push_back(texture);
			material->SetMapKd(texture);
		}
		ss.clear();
		ss.str(std::string());
	}

	return true;
}

void TriangleMesh::LoadVertexIndices(std::stringstream& ss, SubMesh* sub_mesh,
	std::unordered_map<std::string, unsigned int>& unique_vertices,
	std::vector<glm::vec3>& positions,
	std::vector<glm::vec3>& normals,
	std::vector<glm::vec2>& uvs)
{
	numTriangles++;
	int first_vertex_index; // For Polygon first vertex indices
	int temp_vertex_count = 0; // if vertex_count > 3, it is a polygon
	// vertex_index[0]: position index; vertex_index[1]: texcoord index; vertex_index[2]: normal index
	int vertex_index[3] = { -1, -1, -1 };
	std::string vertex_index_str;
	while (!ss.eof()) {
		temp_vertex_count++;
		for (int i = 0; i < 3; i++) {
			if (ss.peek() == '/') { // Handle cases like "f 1//1 2//2 3//3"
				vertex_index[i] = -1;
				ss.ignore();
				continue;
			}
			ss >> vertex_index[i];
			if (ss.peek() == '/') ss.ignore(); // Ignore '/' after reading index
		}

		if (temp_vertex_count > 3) { // Polygon: create two vertices for triangle
			int last_vertex_index = sub_mesh->vertexIndices.back();
			sub_mesh->vertexIndices.push_back(first_vertex_index);
			sub_mesh->vertexIndices.push_back(last_vertex_index);
			numTriangles++;
		}

		vertex_index_str = std::to_string(vertex_index[0]) + "/" +
			std::to_string(vertex_index[1]) + "/" + std::to_string(vertex_index[2]);
		std::unordered_map<std::string, unsigned int>::iterator it = unique_vertices.find(vertex_index_str);
		if (it == unique_vertices.end()) { // New vertex
			VertexPTN newVertex;
			if (vertex_index[1] == -1) // No texcoord
				newVertex = VertexPTN(positions[vertex_index[0] - 1], normals[vertex_index[2] - 1], glm::vec2(0.0f, 0.0f));
			else
				newVertex = VertexPTN(positions[vertex_index[0] - 1], normals[vertex_index[2] - 1], uvs[vertex_index[1] - 1]);
			vertices.push_back(newVertex);
			unique_vertices[vertex_index_str] = numVertices;
			sub_mesh->vertexIndices.push_back(numVertices++);
		}
		else { // Existing vertex
			sub_mesh->vertexIndices.push_back(it->second);
		}

		if (temp_vertex_count == 1) { // For Polygon first vertex index
			first_vertex_index = sub_mesh->vertexIndices.back();
		}
	}
}

void TriangleMesh::CreateVertexBuffer() {
	if (glIsBuffer(vboId)) return;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexPTN), &vertices[0], GL_STATIC_DRAW);
}

void TriangleMesh::CreateIndexBuffer() {
	for (SubMesh& sub_mesh : subMeshes) {
		if (glIsBuffer(sub_mesh.iboId)) continue;
		glGenBuffers(1, &(sub_mesh.iboId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub_mesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * sub_mesh.vertexIndices.size(),
			&(sub_mesh.vertexIndices[0]), GL_STATIC_DRAW);
	}
}

void TriangleMesh::Render(SubMesh& sub_mesh) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2); // Texcoord

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub_mesh.iboId);
	glDrawElements(GL_TRIANGLES, sub_mesh.vertexIndices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}

void TriangleMesh::UpdateWorldMatrix() {
	glm::mat4x4 S = glm::scale(glm::mat4(1.0f), scale_vector_);
	glm::mat4x4 R = rotate_matrix_;
	glm::mat4x4 T = glm::translate(glm::mat4(1.0f), translate_vector_);
	world_matrix_ = T * R * S; // TRANSLATION MUST APPLY LAST
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

