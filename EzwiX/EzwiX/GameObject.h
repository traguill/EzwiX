#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include <string>

class Component;

//NOTE: This class is empty for now. It exists only to avoid errors with other classes. (I will fill it later)

class GameObject
{
public:
	GameObject();
	~GameObject();

	GameObject(GameObject* parent);

	void RemoveComponent(Component* comp);
	
	bool AddChild(GameObject* child);
	bool RemoveChild(GameObject* child);
	const std::vector<GameObject*>* GetChilds()const;
	unsigned int ChildCount()const;

	unsigned int GetUUID()const;

private:

	void CleanUp();

private:

	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;

	unsigned int uuid;

public:
	std::string name;

};

#endif // !__GAMEOBJECT_H__
