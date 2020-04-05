#pragma once
#include "Rendering/Mesh.h"
#include "glm.hpp"

//Assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class MeshLoader
{
public:
	static std::vector<Mesh> LoadModel(const std::string& filepath);

private:
	static void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
	static Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	static void FillVector3(glm::vec3& vec3, const aiVector3D& vector3D);
	static void FillVector2(glm::vec2& vec2, const aiVector3D& vector3D);
};