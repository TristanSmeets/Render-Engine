#include "Rendererpch.h"
#include "Scene.h"

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
}

void Scene::InitializeMeshes()
{
}

void Scene::InitializeMaterials()
{
}

void Scene::InitializeActors()
{
}
