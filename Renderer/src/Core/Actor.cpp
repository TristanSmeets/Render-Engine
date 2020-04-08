#include "Rendererpch.h"
#include "Actor.h"

Actor::Actor(const std::string & name, const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale) :
	name(name)
{
	transform.Rotate(rotation);
	transform.Translate(position);
	transform.Scale(scale);
}

Actor::Actor(const Actor & rhs) :
	name(rhs.name), transform(rhs.transform), children(rhs.children), parent(rhs.parent),
	renderComponent(rhs.renderComponent)
{
}

Actor::~Actor()
{
	if (parent != nullptr)
	{
		parent->RemoveChild(*this);
	}

	for (int i = 0; i < children.size(); ++i)
	{
		children[i]->RemoveParent();
	}
}

void Actor::AddChild(Actor & actor)
{
	children.push_back(&actor);
	actor.SetParent(*this);
}

const unsigned int Actor::GetChildCount()
{
	return (unsigned int)children.size();
}

Actor & Actor::GetChild(unsigned int index)
{
	return *children[index];
}

void Actor::RemoveChild(const Actor & actor)
{
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == &actor)
		{
			children[i]->RemoveParent();
			children.erase(children.begin() + i);
			return;
		}
	}
}

void Actor::SetParent(Actor & actor)
{
	parent = &actor;
}

const Actor & Actor::GetParent()
{
	return *parent;
}

void Actor::RemoveParent()
{
	parent = nullptr;
}

const std::string & Actor::GetName() const
{
	return name;
}

Transform & Actor::GetTransform()
{
	return transform;
}

const Transform & Actor::GetTransform() const
{
	return transform;
}

const glm::mat4 Actor::GetWorldMatrix() const
{

	if (parent == nullptr)
	{
		return transform.GetMatrix();
	}
	else
	{
		return parent->GetWorldMatrix() * transform.GetMatrix();
	}
}

const glm::vec3 Actor::GetWorldPosition() const
{
	if (parent == nullptr)
	{
		return transform.GetPosition();
	}
	else
	{
		return parent->GetWorldPosition() + transform.GetPosition();
	}
}

const RenderComponent & Actor::GetRenderComponent() const
{
	return renderComponent;
}

RenderComponent & Actor::GetRenderComponent()
{
	return renderComponent;
}
