#include "Rendererpch.h"
#include "MeshLoader.h"

std::vector<Mesh> MeshLoader::LoadModel(const std::string & filepath)
{
	std::vector<Mesh> meshes;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return meshes;
	}

	processNode(scene->mRootNode, scene, meshes);
	return meshes;
}

void MeshLoader::processNode(aiNode * node, const aiScene * scene, std::vector<Mesh>& meshes)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* tempMesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(tempMesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene, meshes);
	}
}

Mesh MeshLoader::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	glm::vec3 placeholder;
	glm::vec2 uv;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		//Vertices
		FillVector3(placeholder, mesh->mVertices[i]);
		vertices.push_back(placeholder);

		//Normals
		FillVector3(placeholder, mesh->mNormals[i]);
		normals.push_back(placeholder);

		//UVs
		if (mesh->HasTextureCoords)
		{
			FillVector2(uv, mesh->mTextureCoords[0][i]);
			uvs.push_back(uv);
		}
		else
		{
			uvs.push_back(glm::vec2(0.0f));
		}
	}

	//Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh::Data meshInfo(vertices, normals, uvs, indices);

	return Mesh(mesh->mName.C_Str(), meshInfo);
}

void MeshLoader::FillVector3(glm::vec3 & vec3, const aiVector3D & vector3D)
{
	vec3.x = vector3D.x;
	vec3.y = vector3D.y;
	vec3.z = vector3D.z;
}

void MeshLoader::FillVector2(glm::vec2 & vec2, const aiVector3D & vector3D)
{
	vec2.x = vector3D.x;
	vec2.y = vector3D.y;
}
