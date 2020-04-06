#pragma once

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include "Rendering/Skybox.h"

#include "Core/Actor.h"
#include "Core/Light.h"
#include "Core/Camera.h"
#include "Core/Window.h"

class Scene
{
public:
	Scene(Window& window);
	virtual ~Scene();
	const std::vector<Actor>& GetActors() const;
	const std::vector<Light>& GetLights() const;
	const Camera& GetCamera() const;
	const Skybox& GetSkybox() const;
	virtual void Initialize();

protected:
	virtual void InitializeMeshes() = 0;
	virtual void InitializeMaterials() = 0;
	virtual void InitializeActors() = 0;

	std::vector<Actor> actors;
	std::vector<Light> lights;
	std::vector<Material> materials;
	std::vector<Mesh> meshes;
	Camera camera;
	Skybox skybox;
};