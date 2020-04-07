#include "Rendererpch.h"
#include "Scene.h"
#include "Utility/MeshLoader.h"
#include "Utility/Filepath.h"

static int counter = 0;

Scene::Scene()
{
}

Scene::Scene(Window & window) :
	camera(Camera(window.GetGLFWwindow()))
{
	printf("Creating Scene\n");
}

Scene::Scene(const Scene & rhs) :
	actors(rhs.actors), lights(rhs.lights),
	materials(rhs.materials), meshes(rhs.meshes),
	camera(rhs.camera), skybox(rhs.skybox)
{
}

Scene::~Scene()
{
	printf("Destroying Scene\n");
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

Camera & Scene::GetCamera()
{
	return camera;
}

const Skybox & Scene::GetSkybox() const
{
	return skybox;
}

void Scene::Initialize()
{
	printf("\nInitializing Scene\n");
	InitializeMeshes();
	InitializeMaterials();
	InitializeActors();
	skybox.Initialize();
	skybox.LoadHDR(Filepath::Skybox + "Tropical_Beach/Tropical_Beach_3k.hdr");
	printf("Initializing complete\n\n");
}

Scene & Scene::operator=(const Scene & rhs)
{
	this->actors = rhs.actors;
	this->lights = rhs.lights;
	this->materials = rhs.materials;
	this->meshes = rhs.meshes;
	this->camera = rhs.camera;
	this->skybox = rhs.skybox;
	return *this;
}

void Scene::InitializeMeshes()
{
	printf("Initializing Meshes\n");
	const std::vector<Mesh>& lampMeshes = MeshLoader::LoadModel(Filepath::Mesh + "cube.obj");
	for (unsigned int i = 0; i < lampMeshes.size(); ++i)
	{
		meshes.push_back(lampMeshes[i]);
	}

	const std::vector<Mesh>& sphereMeshes = MeshLoader::LoadModel(Filepath::Mesh + "sphere.obj");
	for (unsigned int i = 0; i < sphereMeshes.size(); ++i)
	{
		meshes.push_back(sphereMeshes[i]);
	}
	printf("Created %d meshes\n", (int)meshes.size());
}

void Scene::InitializeMaterials()
{
	printf("Initializing Materials\n");
	Material material = Material("Sphere");
	material.AddTexture(Texture::Albedo, Filepath::Texture + "Aluminium/Albedo.png", true);
	material.AddTexture(Texture::Normal, Filepath::Texture + "Aluminium/Normal.png");
	material.AddTexture(Texture::Metallic, Filepath::Texture + "Aluminium/Metallic.png");
	material.AddTexture(Texture::Roughness, Filepath::Texture + "Aluminium/Roughness.png");
	material.AddTexture(Texture::AmbientOcclusion, Filepath::Texture + "Aluminium/Mixed_AO.png");
	materials.push_back(material);

	printf("Created %d materials\n", (int)materials.size());
}

void Scene::InitializeActors()
{
	printf("Initializing actors\n");
	Light light = Light("Light", glm::vec3(-5.0f, 6.0f, 4.0f), glm::vec3(81.0f, 57.0f, 11.0f));
	light.GetRenderComponent().SetMesh(meshes[0]);
	lights.push_back(light);

	Actor sphere = Actor("Sphere");
	sphere.GetRenderComponent().SetMesh(meshes[1]);
	sphere.GetRenderComponent().SetMaterial(materials[0]);
	actors.push_back(sphere);
	printf("Created %d actors\n", (int)actors.size());
}
