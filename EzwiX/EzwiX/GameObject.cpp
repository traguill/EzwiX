#include "GameObject.h"

#include "Application.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#include "Random.h"
#include "log.h"

GameObject::GameObject()
{
	Init();
}

GameObject::~GameObject()
{
	for (std::vector<Component*>::iterator component = components.begin(); component != components.end(); ++component)
	{
		delete (*component);
		(*component) = nullptr;
	}

	components.clear();
	components_to_remove.clear();
}

GameObject::GameObject(GameObject * parent)
{
	Init();

	if(parent)
	{
		this->parent = parent;
		parent->AddChild(this);
	}
}

GameObject::GameObject(const char * name, unsigned int uuid, GameObject * parent) : name(name), uuid(uuid), parent(parent)
{
	//NOTE:This method is only to initialize the gameobject from a 3D model file. This constructor doesn't automatically create the transform component, neither links the parent with the childs. It's done all outside.
}

void GameObject::PreUpdate()
{
	//Remove all components that need to be removed. Secure way.
	for (std::vector<Component*>::iterator component = components_to_remove.begin(); component != components_to_remove.end(); ++component)
	{
		for (std::vector<Component*>::iterator comp_remove = components.begin(); comp_remove != components.end(); ++comp_remove) //Remove the component from the components list
		{
			if ((*comp_remove) == (*component))
			{
				components.erase(comp_remove);
				break;
			}
		}
		delete (*component);
	}

	components_to_remove.clear();
}

void GameObject::Update()
{
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); comp++)
	{
		(*comp)->PreUpdate();
	}
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); comp++)
	{
		(*comp)->Update();
	}
	for (std::vector<Component*>::iterator comp = components.begin(); comp != components.end(); comp++)
	{
		(*comp)->PostUpdate();
	}
}

Component * GameObject::AddComponent(ComponentType type)
{
	Component* item = nullptr;
	switch (type)
	{
	case C_TRANSFORM:
		if (!transform) //Only one transform component for gameobject
		{
			item = new ComponentTransform(type, this, &global_matrix);
			transform = (ComponentTransform*)item;
		}
		break;
	case C_MESH:
		if (transform)
		{
			item = new ComponentMesh(type, this);
		}
		break;
	default:
		LOG("[WARNING] Unknown type specified for GameObject %s", name);
		break;
	}

	if (item != nullptr)
	{
		components.push_back(item);
	}
	else
	{
		LOG("[ERROR] When adding component to %s", this->name.c_str());
	}

	return item;
}

Component* GameObject::GetComponent(ComponentType type) const
{
	if (components.empty() == false)
	{
		if (type == C_TRANSFORM)
			return transform;

		std::vector<Component*>::const_iterator comp = components.begin();
		while (comp != components.end())
		{
			if ((*comp)->GetType() == type)
			{
				return (*comp);
			}
			++comp;
		}
	}
	return nullptr;
}

const std::vector<Component*>* GameObject::GetComponents()
{
	return &components;
}

void GameObject::RemoveComponent(Component * comp)
{
	std::vector<Component*>::iterator it = components.begin();

	for (it; it != components.end(); ++it)
	{
		if ((*it) == comp)
		{
			components_to_remove.push_back(comp);
			break;
		}
	}
}

bool GameObject::AddChild(GameObject * child)
{
	bool ret = false;

	if (child)
	{
		childs.push_back(child);
		ret = true;
	}

	return ret;
}

bool GameObject::RemoveChild(GameObject * child)
{
	bool ret = false;
	if (child)
	{
		for(std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
			if ((*go) == child)
			{
				childs.erase(go);
				break;
			}
	}
	return ret;
}

const std::vector<GameObject*>* GameObject::GetChilds() const
{
	return &childs;
}

unsigned int GameObject::ChildCount() const
{
	return childs.size();
}

GameObject * GameObject::GetParent() const
{
	return parent;
}

unsigned int GameObject::GetUUID() const
{
	return uuid;
}

bool GameObject::IsActive() const
{
	return active;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

void GameObject::TransformModified()
{
	if (global_matrix == nullptr)
		return;
	std::vector<Component*>::iterator component = components.begin();

	for (component; component != components.end(); component++)
	{
		(*component)->OnTransformModified();
	}
}

void GameObject::Init()
{
	name.resize(32);
	name = "GameObject"; //Name by default
	uuid = App->rnd->RandomInt();

	AddComponent(C_TRANSFORM);
}

void GameObject::CleanUp()
{
	if (parent)
	{
		parent->RemoveChild(this);
	}

	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
		delete (*go);

	childs.clear();
}
