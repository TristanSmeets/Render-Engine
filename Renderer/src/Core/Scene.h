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
	Scene();
	Scene(Window& window);
	Scene(const Scene& rhs);
	~Scene();
	const std::vector<Actor>& GetActors() const;
	std::vector<Actor>& GetActors();
	const std::vector<Light>& GetLights() const;
	const Camera& GetCamera() const;
	Camera& GetCamera();
	const Skybox& GetSkybox() const;
	void Initialize();
	const unsigned int& GetNumberOfLights() const;
	Scene& operator=(const Scene& rhs);

private:
	void InitializeMeshes();
	void InitializeMaterials();
	void InitializeActors();

	const unsigned int MaximumNumberOfLights = 10;
	unsigned int NumberOfLights = 3;
	std::vector<Actor> actors;
	std::vector<Light> lights;
	std::vector<Material> materials;
	std::vector<Mesh> meshes;
	Camera camera;
	Skybox skybox;
};