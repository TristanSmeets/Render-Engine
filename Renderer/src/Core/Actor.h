#pragma once

#include "glm.hpp"
#include "Core/Transform.h"
#include "Rendering/RenderComponent.h"

class Actor
{
public:
    Actor(const std::string& name = "Actor", const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
    Actor(const Actor& rhs);
    virtual ~Actor();
    virtual void AddChild(Actor& actor);
    virtual const unsigned int GetChildCount();
    virtual Actor& GetChild(unsigned int index);
    virtual void RemoveChild(const Actor& actor);
    virtual void SetParent(Actor& actor);
    virtual const Actor& GetParent();
    virtual void RemoveParent();
	virtual void SetName(const std::string& name);
    virtual const std::string& GetName() const;
    virtual const Transform& GetTransform() const;
    virtual Transform& GetTransform();
    virtual const glm::mat4 GetWorldMatrix() const;
	virtual const glm::vec3 GetWorldPosition() const;
	virtual const RenderComponent& GetRenderComponent() const;
	virtual RenderComponent& GetRenderComponent();

protected:
    Actor* parent = nullptr; 
    Transform transform = Transform();
	RenderComponent renderComponent;
    std::vector<Actor*> children;
    std::string name = "";
    
};