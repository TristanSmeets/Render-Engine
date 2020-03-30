#include "Core/Actor.h"
#include "Core/Light.h"

#include <iostream>

int main()
{
    Actor parent = Actor("Parent");
    std::cout << "Actor1: " << parent.GetName() << std::endl;

    Actor child = Actor("Child", glm::vec3(1.0f));
    std::cout << "Actor2: " << child.GetName() << std::endl;

    parent.AddChild(child);

    std::cout << "Parent child count: " << parent.GetChildCount() << std::endl;

    Actor copyChild = parent.GetChild(parent.GetChildCount() - 1);

    std::cout << "CopyChild: " << copyChild.GetName() << std::endl;

    std::cout << "CopyChild parent: " << copyChild.GetParent().GetName() << std::endl;

    Light light = Light("Light",glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.4f,0.5f,1.0f));

    light.GetTransform().Translate(glm::vec3(0.0f, 1.0f, 0.0f));

    parent.AddChild(light);

    parent.GetTransform().Translate(glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 parentPosition = parent.GetWorldMatrix()[3];
    glm::vec3 lightPosition = light.GetWorldMatrix()[3];

    std::cout << "Done" << std::endl;
}