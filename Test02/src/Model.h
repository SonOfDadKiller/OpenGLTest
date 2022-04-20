#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Texture
{
	unsigned int id;
	enum TextureType { Diffuse, Specular };
	TextureType type;
	std::string path;

	Texture() { };
	Texture(TextureType type, unsigned int id)
	{
		this->type = type;
		this->id = id;
	}
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(unsigned int shader);
private:
	unsigned int vao, vbo, ebo;

	void SetupMesh();
};

class Model
{
public:
	Model(const char* path);
	Model(float* meshVertices, int numVertices, unsigned int* meshIndices, int numIndices);
	void Draw(unsigned int shader);
	// model data
	std::vector<Mesh> meshes;
private:
	std::string directory;

	std::vector<Texture> loadedTextures;

	void LoadModel(std::string path);
	void LoadNode(aiNode* node, const aiScene* scene, std::string path);
	Mesh LoadMesh(aiMesh* mesh, const aiScene* scene, std::string path);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType myType, std::string path);
};