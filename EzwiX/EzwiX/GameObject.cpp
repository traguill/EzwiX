#include "GameObject.h"

#include "Application.h"

#include "Random.h"

GameObject::GameObject()
{
	name.resize(32);
	name = "GameObject"; //Name by default
	uuid = App->rnd->RandomInt();
}

GameObject::~GameObject()
{

}

GameObject::GameObject(GameObject * parent)
{
	name.resize(32);
	name = "GameObject"; //Name by default
	uuid = App->rnd->RandomInt();

	if(parent)
	{
		this->parent = parent;
		parent->AddChild(this);
	}
}

void GameObject::RemoveComponent(Component * comp)
{
	//TODO
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

unsigned int GameObject::GetUUID() const
{
	return uuid;
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
