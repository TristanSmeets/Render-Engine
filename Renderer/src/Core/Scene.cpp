#include "Rendererpch.h"
#include "Scene.h"
#include "Utility/MeshLoader.h"
#include "Utility/Filepath.h"
#include "Utility/NDCQuad.h"

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

std::vector<Actor>& Scene::GetActors()
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

	const std::vector<Mesh>& cubeMeshes = MeshLoader::LoadModel(Filepath::Mesh + "cube.obj");
	for (unsigned int i = 0; i < cubeMeshes.size(); ++i)
	{
		meshes.push_back(cubeMeshes[i]);
	}

	const std::vector<Mesh>& sphereMeshes = MeshLoader::LoadModel(Filepath::Mesh + "Demo.obj");
	for (unsigned int i = 0; i < sphereMeshes.size(); ++i)
	{
		meshes.push_back(sphereMeshes[i]);
	}

	const std::vector<Mesh>& planeMesh = MeshLoader::LoadModel(Filepath::Mesh + "plane100x100.obj");
	for (unsigned int i = 0; i < planeMesh.size(); ++i)
	{
		meshes.push_back(planeMesh[i]);
	}
	printf("Created %d meshes\n", (int)meshes.size());
}

void Scene::InitializeMaterials()
{
	printf("Initializing Materials\n");
	Material aluminium = Material("Aluminium");
	aluminium.AddTexture(Texture::Albedo, Filepath::Texture + "Aluminium/Albedo.png", true);
	aluminium.AddTexture(Texture::Normal, Filepath::Texture + "Aluminium/Normal.png");
	aluminium.AddTexture(Texture::Metallic, Filepath::Texture + "Aluminium/Metallic.png");
	aluminium.AddTexture(Texture::Roughness, Filepath::Texture + "Aluminium/Roughness.png");
	aluminium.AddTexture(Texture::AmbientOcclusion, Filepath::Texture + "Aluminium/Mixed_AO.png");

	Material rustedIron = Material("Rusted_Iron");
	rustedIron.AddTexture(Texture::Albedo, Filepath::Texture + "RustedIron/Albedo.png", true);
	rustedIron.AddTexture(Texture::Normal, Filepath::Texture + "RustedIron/Normal.png");
	rustedIron.AddTexture(Texture::Metallic, Filepath::Texture + "RustedIron/Metallic.png");
	rustedIron.AddTexture(Texture::Roughness, Filepath::Texture + "RustedIron/Roughness.png");
	rustedIron.AddTexture(Texture::AmbientOcclusion, Filepath::Texture + "RustedIron/AmbientOcclusion.png");

	Material cobblestone = Material("Cobblestone");
	cobblestone.AddTexture(Texture::Albedo,			Filepath::Texture + "cobblestone/Albedo.png", true);
	cobblestone.AddTexture(Texture::Normal,			Filepath::Texture + "cobblestone/Normal.png");
	cobblestone.AddTexture(Texture::Metallic,			Filepath::Texture + "cobblestone/Metallic.png");
	cobblestone.AddTexture(Texture::Roughness,			Filepath::Texture + "cobblestone/Roughness.png");
	cobblestone.AddTexture(Texture::AmbientOcclusion,	Filepath::Texture + "cobblestone/AmbientOcclusion.png");

	Material demo = Material("Demo");
	demo.AddTexture(Texture::Albedo,			Filepath::Texture + "Demo/Albedo.png", true);
	//demo.AddTexture(Texture::Albedo,			Filepath::Texture + "Aluminium/Albedo.png", true);
	demo.AddTexture(Texture::Normal,			Filepath::Texture + "Demo/Normal.png");
	demo.AddTexture(Texture::Metallic,			Filepath::Texture + "Demo/Metallic.png");
	//demo.AddTexture(Texture::Metallic,			Filepath::Texture + "Aluminium/Metallic.png");
	demo.AddTexture(Texture::Roughness,			Filepath::Texture + "Demo/Roughness.png");
	//demo.AddTexture(Texture::Roughness,			Filepath::Texture + "Auminium/Roughness.png");
	demo.AddTexture(Texture::AmbientOcclusion,	Filepath::Texture + "Demo/AmbientOcclusion_2.png");

	Material leaf = Material("Window");
	leaf.AddTexture(Texture::Albedo, Filepath::Texture + "Leaf/blending_transparent_window.png", true);

	materials.push_back(aluminium);
	materials.push_back(rustedIron);
	materials.push_back(cobblestone);
	materials.push_back(demo);
	materials.push_back(leaf);

	printf("Created %d materials\n", (int)materials.size());
}

void Scene::InitializeActors()
{
	printf("Initializing actors\n");

	const unsigned int NumberOfLights = 10;

	srand(NumberOfLights);

	for (unsigned int i = 0; i < NumberOfLights; ++i)
	{
		float x = ((rand() % 100) / 100.0f) * 30.0f - 10.0f;
		float y = ((rand() % 100) / 100.0f) * 5.0f + 12.0f;
		float z = ((rand() % 100) / 100.0f) * 30.0f - 12.0f;

		float r = ((rand() % 100) / 200.0f) + 0.5f;
		float g = ((rand() % 100) / 200.0f) + 0.5f;
		float b = ((rand() % 100) / 200.0f) + 0.5f;

		Light::Parameters parameters(glm::vec3(r, g, b), 1.0f, 0.14f, 0.07f);

		Light light = Light("Light#" + std::to_string(i), glm::vec3(x, y, z), parameters);
		light.GetRenderComponent().SetMesh(meshes[0]);
		lights.push_back(light);
	}

	Actor sphere = Actor("Demo_Material#3");
	sphere.GetTransform().Translate(glm::vec3(0, 6, 0));
	sphere.GetRenderComponent().SetMesh(meshes[1]);
	sphere.GetRenderComponent().SetMaterial(materials[3]);
	actors.push_back(sphere);
	
	sphere.SetName("Demo_Material#0");
	sphere.GetTransform().Translate(glm::vec3(15, 0, 0));
	sphere.GetRenderComponent().SetMaterial(materials[0]);
	actors.push_back(sphere);

	//for (int i = 0; i < 5; ++i)
	//{
	//	for (int j = 0; j < 5; ++j)
	//	{
	//		std::string name = std::string("Object[") + std::to_string(i) + std::string("][") + std::to_string(j) + std::string("]");
	//		Actor sphere = Actor(name);
	//		sphere.GetTransform().Translate(glm::vec3((i * 4.0f) - 2.5f, 0,(j * 3.0f) - 2.5f));
	//		sphere.GetRenderComponent().SetMesh(meshes[1]);
	//		sphere.GetRenderComponent().SetMaterial(materials[(i + j) % 2]);
	//		actors.push_back(sphere);
	//	}
	//}

	Actor terrain = Actor("Terrain");
	terrain.GetTransform().Translate(glm::vec3(0.0f, -1.0f, 0.0f));
	terrain.GetRenderComponent().SetMesh(meshes[2]);
	terrain.GetRenderComponent().SetMaterial(materials[2]);
	actors.push_back(terrain);

	Actor windowPlane = Actor("Window1");
	windowPlane.GetTransform().Translate(glm::vec3(0.0f, 4.0f, 15.0f));
	windowPlane.GetTransform().Rotate(glm::vec3(90.0f, 0.0f, 0.0f));
	windowPlane.GetTransform().Scale(glm::vec3(0.1f));
	windowPlane.GetRenderComponent().SetMesh(meshes[2]);
	windowPlane.GetRenderComponent().SetMaterial(materials[4]);
	windowPlane.GetRenderComponent().GetADSParameters().IsTransparent = true;
	windowPlane.GetRenderComponent().GetPBRParameters().IsTransparent = true;

	actors.push_back(windowPlane);
	windowPlane.GetTransform().Translate(glm::vec3(5.0f, 0.0f, -2.0f));
	windowPlane.SetName("Window2");
	actors.push_back(windowPlane);

	printf("Created %d actors\n", (int)actors.size());
}
