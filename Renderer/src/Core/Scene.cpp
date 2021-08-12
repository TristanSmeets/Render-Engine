#include "Rendererpch.h"
#include "Scene.h"
#include "Utility/MeshLoader.h"
#include "Utility/Filepath.h"

static int counter = 0;

Scene::Scene() :
	camera(Camera(glm::vec3(17.75f, 8.10f, 29.0f)))
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

std::vector<Light>& Scene::GetLights()
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

const unsigned int& Scene::GetNumberOfLights() const
{
	return NumberOfLights;
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

	const std::vector<Mesh>& DemoMeshes = MeshLoader::LoadModel(Filepath::Mesh + "Demo.obj");
	for (unsigned int i = 0; i < DemoMeshes.size(); ++i)
	{
		meshes.push_back(DemoMeshes[i]);
	}

	const std::vector<Mesh>& planeMesh = MeshLoader::LoadModel(Filepath::Mesh + "plane100x100.obj");
	for (unsigned int i = 0; i < planeMesh.size(); ++i)
	{
		meshes.push_back(planeMesh[i]);
	}

	const std::vector<Mesh>& sphereMesh = MeshLoader::LoadModel(Filepath::Mesh + "sphere.obj");
	for (unsigned int i = 0; i < sphereMesh.size(); ++i)
	{
		meshes.push_back(sphereMesh[i]);
	}
	printf("Created %d meshes\n", (int)meshes.size());
}

void Scene::InitializeMaterials()
{
	printf("Initializing Materials\n");
	Material aluminium = Material("Aluminium");
	aluminium.AddTexture(Texture::Albedo, Filepath::Texture + "Aluminium/Albedo.png", true);
	aluminium.AddTexture(Texture::Normal, Filepath::Texture + "Aluminium/Normal.png");
	aluminium.AddTexture(Texture::MRAO, Filepath::Texture + "Aluminium/MRAO.png");

	Material rustedIron = Material("Rusted_Iron");
	rustedIron.AddTexture(Texture::Albedo, Filepath::Texture + "RustedIron/Albedo.png", true);
	rustedIron.AddTexture(Texture::Normal, Filepath::Texture + "RustedIron/Normal.png");
	rustedIron.AddTexture(Texture::MRAO, Filepath::Texture + "RustedIron/MRAO.png");

	Material cobblestone = Material("Cobblestone");
	cobblestone.AddTexture(Texture::Albedo, Filepath::Texture + "cobblestone/Albedo.png", true);
	cobblestone.AddTexture(Texture::Normal, Filepath::Texture + "cobblestone/Normal.png");
	cobblestone.AddTexture(Texture::MRAO, Filepath::Texture + "cobblestone/MRAO.png");

	Material demo = Material("Demo");
	demo.AddTexture(Texture::Albedo, Filepath::Texture + "Demo/Albedo.png", true);
	demo.AddTexture(Texture::Normal, Filepath::Texture + "Demo/Normal.png");
	demo.AddTexture(Texture::MRAO, Filepath::Texture + "Demo/MRAO.png");

	materials.push_back(aluminium);
	materials.push_back(rustedIron);
	materials.push_back(cobblestone);
	materials.push_back(demo);

	printf("Created %d materials\n", (int)materials.size());
}

void Scene::InitializeActors()
{
	printf("Initializing actors\n");
	srand(NumberOfLights);

	if (NumberOfLights > MaximumNumberOfLights)
	{
		NumberOfLights = MaximumNumberOfLights;
	}

	for (unsigned int i = 0; i < MaximumNumberOfLights; ++i)
	{
		//float x = ((rand() % 100) / 100.0f) * 30.0f - 10.0f;
		float x = ((rand() % 100) / 100.0f) * 30.0f - 15.0f;
		//float y = ((rand() % 100) / 100.0f) * 5.0f + 12.0f;
		float y = ((rand() % 100) / 100.0f) * 5.0f + 5.0f;
		float z = ((rand() % 100) / 100.0f) * 30.0f - 12.0f;

		float r = ((rand() % 100) / 10.0f);// + 0.5f;
		float g = ((rand() % 100) / 10.0f);// + 0.5f;
		float b = ((rand() % 100) / 10.0f);// + 0.5f;

		Light::Parameters parameters(glm::vec3(r, g, b));
		parameters.CircleRadius = (float)(rand() % 20);
		parameters.StartPosition = glm::vec3(x, y, z);
		if (i % 2 == 0)
		{
			parameters.isRotatingClockwise = true;
		}

		Light light = Light("Light#" + std::to_string(i), glm::vec3(x, y, z), parameters);
		light.GetTransform().Scale(glm::vec3(0.1f));
		light.GetRenderComponent().SetMesh(meshes[0]);
		lights.push_back(light);
	}

	Actor sphere = Actor("Normal");
	sphere.GetTransform().Translate(glm::vec3(0, 6, 0));
	sphere.GetRenderComponent().SetMesh(meshes[1]);
	sphere.GetRenderComponent().SetMaterial(materials[3]);
	actors.push_back(sphere);

	//sphere.SetName("Aluminium");
	//sphere.GetTransform().Translate(glm::vec3(15, 0, 0));
	//sphere.GetRenderComponent().SetMaterial(materials[0]);
	//sphere.GetRenderComponent().GetPBRParameters().UsingSmoothness = false;
	//actors.push_back(sphere);
	//
	//sphere.SetName("Rusted Iron");
	//sphere.GetTransform().SetPosition(glm::vec3(-15, 6, 0));
	//sphere.GetRenderComponent().SetMaterial(materials[1]);
	//actors.push_back(sphere);

	//sphere.SetName("Cobblestone");
	//sphere.GetTransform().Translate(glm::vec3(-15, 0, 0));
	//sphere.GetRenderComponent().SetMaterial(materials[2]);
	//actors.push_back(sphere);

	float radius = 15;
	int counter = 0;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			std::string name = std::string("Object[") + std::to_string(i) + std::string("][") + std::to_string(j) + std::string("]");
			Actor sphere = Actor(name);
			//glm::vec3 position = glm::vec3(cos(i + j) * radius, 2, sin(i + j) * radius);
			glm::vec3 position = glm::vec3(cos(counter + i) * radius, 2, sin(counter + i) * radius);
			sphere.GetTransform().Translate(position);
			//sphere.GetTransform().Translate(glm::vec3((i * 4.0f) - 2.5f, 0, (j * 3.0f) - 2.5f));
			sphere.GetTransform().Scale(glm::vec3(2.0f));
			sphere.GetRenderComponent().SetMesh(meshes[3]);
			sphere.GetRenderComponent().SetMaterial(materials[(i + j) % 2]);
			actors.push_back(sphere);
			++counter;
		}
	}

	Actor terrain = Actor("Cobblestone");
	terrain.GetTransform().Translate(glm::vec3(0.0f, -1.0f, 0.0f));
	terrain.GetRenderComponent().SetMesh(meshes[2]);
	terrain.GetRenderComponent().SetMaterial(materials[2]);
	actors.push_back(terrain);

	printf("Created %d actors\n", (int)actors.size());
}
