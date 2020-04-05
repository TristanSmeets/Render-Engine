#include "Rendererpch.h"
#include "Scene.h"

Scene::Scene()
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
