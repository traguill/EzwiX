#include "ComponentTexture.h"

#include "Data.h"
#include "log.h"
#include "Globals.h"

ComponentTexture::ComponentTexture(ComponentType type, GameObject * game_object) : Component(type, game_object)
{
}

ComponentTexture::~ComponentTexture()
{
}

void ComponentTexture::Save(Data & file) const
{
	Data data;
	data.AppendInt("type", type);
	data.AppendUInt("UUID", uuid);
	data.AppendBool("active", active);

	//TODO: Save specific texture info

	file.AppendArrayValue(data);
}

void ComponentTexture::Load(Data & conf)
{
	uuid = conf.GetUInt("UUID");
	active = conf.GetBool("active");

	//TODO: Load specific texture info
}

bool ComponentTexture::Initialize(ID3D11Device * device, const char * tex_path)
{
	HRESULT result;

	//Load texture
	result = D3DX11CreateShaderResourceViewFromFile(device, tex_path, NULL, NULL, &texture, NULL);
	if (FAILED(result))
	{
		LOG("Component texture ERROR while loading texture %s", tex_path);
		return false;
	}

	return true;
}

ID3D11ShaderResourceView * ComponentTexture::GetTexture() const
{
	return texture;
}

void ComponentTexture::CleanUp()
{
	RELEASE(texture);
}
