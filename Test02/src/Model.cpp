#include <glad.h>
#include <glfw3.h>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include "Texture.h"
#include "Model.h"
#include "Camera.h"

using std::vector;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    SetupMesh();
}

void Mesh::SetupMesh()
{
    //Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Generate, bind and fill VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    //Generate, bind and fill EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    //Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal)); //normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, uv)); //uv
    glEnableVertexAttribArray(2);

    //Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(unsigned int shader)
{
    //Pass textures to shader program
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name;

        switch (textures[i].type)
        {
        case Texture::Diffuse:
            name = "texture_diffuse";
            number = std::to_string(diffuseNr++);
            break;
        case Texture::Specular:
            name = "texture_specular";
            number = std::to_string(specularNr++);
            break;
        }

        unsigned int uniformLocation = glGetUniformLocation(shader, ("material." + name + number).c_str());
        glUniform1i(uniformLocation, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);

    //Draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model(const char* path)
{
    LoadModel(path);
}

Model::Model(float* meshVertices, int numVertices, unsigned int* meshIndices, int numIndices)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < numVertices; i++)
    {
        Vertex vertex;
        vertex.position.x = *(meshVertices);
        vertex.position.y = *(meshVertices + 1);
        vertex.position.z = *(meshVertices + 2);
        vertex.normal.x = *(meshVertices + 3);
        vertex.normal.y = *(meshVertices + 4);
        vertex.normal.z = *(meshVertices + 5);
        vertex.uv.x = *(meshVertices + 6);
        vertex.uv.y = *(meshVertices + 7);

        meshVertices += 8;
        vertices.push_back(vertex);
    }

    for (int i = 0; i < numIndices; i++)
    {
        indices.push_back(*meshIndices);
        meshIndices++;
    }

    meshes.push_back(Mesh(vertices, indices, std::vector<Texture>()));
}

void Model::Draw(unsigned int shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

void Model::LoadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    std::filesystem::path p(path);

    LoadNode(scene->mRootNode, scene, p.parent_path().generic_string() + "/");
}

void Model::LoadNode(aiNode* node, const aiScene* scene, std::string path)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(LoadMesh(mesh, scene, path));
    }

    //std::cout << "child count: " << node->mNumChildren << std::endl;

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(node->mChildren[i], scene, path);
    }
}

Mesh Model::LoadMesh(aiMesh* mesh, const aiScene* scene, std::string path)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;


       /* std::cout << "x=" << vertex.position.x << ", "
                  << "y=" << vertex.position.y << ", "
                  << "z=" << vertex.position.z << std::endl;*/


        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
        }
        else
        {
            vertex.uv = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    
    //Indices - 1 face = 1 index because the mesh is already triangulated
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Diffuse, path);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = LoadMaterialTextures(material,  aiTextureType_SPECULAR, Texture::Specular, path);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType myType, std::string path)
{
    vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString textureFileName;
        mat->GetTexture(type, i, &textureFileName);
        bool skip = false;

        for (unsigned int j = 0; j < loadedTextures.size(); j++)
        {
            if (std::strcmp(loadedTextures[j].path.data(), textureFileName.C_Str()) == 0)
            {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;

            aiString texPath(path.c_str());
            texPath.Append(textureFileName.C_Str());

            texture.id = LoadTexture(texPath.C_Str());
            texture.type = myType;
            texture.path = textureFileName.C_Str();
            textures.push_back(texture);
            loadedTextures.push_back(texture); // add to loaded textures
        }
    }

    return textures;
}