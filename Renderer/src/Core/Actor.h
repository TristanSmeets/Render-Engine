#pragma once

#include "Core/Transform.h"
#include "glm.hpp"

#include <string>
#include <vector>

class Actor
{
public:
    Actor(const std::string& name = "Actor", const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
    Actor(const Actor& rhs);
    ~Actor();
    void AddChild(Actor& actor);
    const unsigned int GetChildCount();
    Actor& GetChild(unsigned int index);
    void RemoveChild(const Actor& actor);
    void SetParent(Actor& actor);
    const Actor& GetParent();
    void RemoveParent();
    const std::string& GetName() const;
    const Transform& GetTransform() const;
    glm::mat4 GetWorldMatrix();

private:
    Actor* parent = nullptr; 
    Transform transform = Transform();
    //Insert RenderComponent
    std::vector<Actor*> children;
    std::string name = "";
    
};