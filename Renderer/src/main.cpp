#include "Core/Actor.h"

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
}