#include "Rendererpch.h"
#include "Scene.h"
#include "Utility/MeshLoader.h"
#include "Utility/Filepath.h"

Scene::Scene()
{
}

Scene::Scene(Window & window) :
	camera(Camera(window.GetGLFWwindow()))
{
}

Scene::Scene(const Scene & rhs) :
	actors(rhs.actors), lights(rhs.lights),
	materials(rhs.materials), meshes(rhs.meshes),
	camera(rhs.camera), skybox(rhs.skybox)
{
}

Scene::~Scene()
{
}


const std::vector<Actor>& Scene::GetActors() const
{
	return actors;
}

const std::vector<Light>& Scene::GetLights() const
{
	return lights;
}

const Camera & Scene::GetCamera() const
{
	return camera;
}

const Skybox & Scene::GetSkybox() const
{
	return skybox;
}

void Scene::Initialize()
{
	InitializeMeshes();
	InitializeMaterials();
	InitializeActors();
}

Scene & Scene::operator=(const Scene & rhs)
{
	actors = rhs.actors;
	lights = rhs.lights;
	materials = rhs.materials;
	meshes = rhs.meshes;
	camera = rhs.camera;
	skybox = rhs.skybox;
	return *this;
}

void Scene::InitializeMeshes()
{
	const std::vector<Mesh>& lampMeshes = MeshLoader::LoadModel(Filepath::Mesh + "cube.obj");
	for (unsigned int i = 0; lampMeshes.size(); ++i)
	{
		meshes.push_back(lampMeshes[i]);
	}

	const std::vector<Mesh>& sphereMeshes = MeshLoader::LoadModel(Filepath::Mesh + "sphere.obj");
	for (unsigned int i = 0; sphereMeshes.size(); ++i)
	{
		meshes.push_back(sphereMeshes[i]);
	}
}

void Scene::InitializeMaterials()
{
	Material material = Material("Sphere");
	material.AddTexture(Texture::Albedo, Filepath::Texture + "Aluminium/Albedo.png", true);
	material.AddTexture(Texture::Normal, Filepath::Texture + "Aluminium/Normal.png");
	material.AddTexture(Texture::Metallic, Filepath::Texture + "Aluminium/Metallic.png");
	material.AddTexture(Texture::Roughness, Filepath::Texture + "Aluminium/Roughness.png");
	material.AddTexture(Texture::AmbientOcclusion, Filepath::Texture + "Aluminium/Mixed_AO.png");
	materials.push_back(material);
}

void Scene::InitializeActors()
{
	Light light = Light("Light", glm::vec3(7.4f, 6.0f, .6f), glm::vec3(81.0f, 57.0f, 11.0f));
	light.GetRenderComponent().SetMesh(meshes[0]);
	lights.push_back(light);

	Actor sphere = Actor("Sphere");
	sphere.GetRenderComponent().SetMesh(meshes[1]);
	sphere.GetRenderComponent().SetMaterial(materials[0]);
	actors.push_back(sphere);
}
