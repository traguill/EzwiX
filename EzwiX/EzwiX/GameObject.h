#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include <string>

#include "MathGeoLib\include\MathGeoLib.h"

class Component;
class ComponentTransform;

enum ComponentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	GameObject(GameObject* parent);
	GameObject(const char* name, unsigned int uuid, GameObject* parent);

	void PreUpdate();
	void Update();

	//Components
	Component* AddComponent(ComponentType type);
	Component* GetComponent(ComponentType type) const;
	const std::vector<Component*>* GetComponents();
	void RemoveComponent(Component* comp);
	
	//Childs
	bool AddChild(GameObject* child);
	bool RemoveChild(GameObject* child);
	const std::vector<GameObject*>* GetChilds()const;
	unsigned int ChildCount()const;

	GameObject* GetParent()const;
	unsigned int GetUUID()const;
	
	//Active
	bool IsActive()const;
	void SetActive(bool active);

	void TransformModified();

private:
	void Init();
	void CleanUp();

private:

	GameObject* parent = nullptr;
	std::vector<GameObject*> childs;

	bool active = true;
	unsigned int uuid;
	float4x4* global_matrix = nullptr;

	std::vector<Component*> components;
	std::vector<Component*> components_to_remove;

public:
	std::string name;

	//Direct access
	ComponentTransform* transform = nullptr;

};

#endif // !__GAMEOBJECT_H__
