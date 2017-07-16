#ifndef __COMPONENT_TEXTURE_H__
#define __COMPONENT_TEXTURE_H__

#include "Component.h"

#include <d3d11.h>
#include <d3dx11tex.h>

#include <string>

class ComponentTexture : public Component
{
public:
	ComponentTexture(ComponentType type, GameObject* game_object);
	~ComponentTexture();

	void Save(Data& file) const;
	void Load(Data& conf);

	bool Initialize(ID3D11Device* device, const char* tex_path);

	ID3D11ShaderResourceView* GetTexture()const;

	std::string GetTexturePath()const;
	void SetTexturePath(const char* path);

private:
	
	void CleanUp();

private:

	ID3D11ShaderResourceView* texture = nullptr;

	std::string texture_path;
};
#endif // !__COMPONENT_TEXTURE_H__
