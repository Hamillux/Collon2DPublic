#include "MasterDataHelper.h"
#include "SystemTypes.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include <DxLib.h>
#include <fstream>
#include <nlohmann/json.hpp>

void MasterHelper::GetGameIconImguiImage(const MasterDataID icon_id, ImTextureID& out_im_tex_id, float& out_u0, float& out_v0, float& out_u1, float& out_v1)
{
	const MdGameIcon& icon =  MdGameIcon::Get(icon_id);
	ID3D11ShaderResourceView* srv = nullptr;
	GraphicResourceManager::GetInstance().GetTexture(icon.image_id, nullptr, &srv);

	if (srv == nullptr)
	{
		throw std::runtime_error("srv is invalid");
		return;
	}
	out_im_tex_id = (void*)srv;
	icon.CalcUV(out_u0, out_v0, out_u1, out_v1);
}

void MasterHelper::GetGameIconImguiImage(const MasterDataID icon_id, ImGui::Texture& texture)
{
	GetGameIconImguiImage(icon_id, texture.im_tex_id, texture.u0(), texture.v0(), texture.u1(), texture.v1());
}

int MasterHelper::GetGameIconHandleForDxLib(const MasterDataID icon_id)
{
	const MdGameIcon& icon = MdGameIcon::Get(icon_id);
	return GraphicResourceManager::GetInstance().GetDerivedGraph(icon.image_id, icon.left, icon.top, icon.width, icon.height);
}

int MasterHelper::GetGameIconHandleForDxLib(const MdItem& md_item)
{
	return GetGameIconHandleForDxLib(md_item.icon_id);
}

int MasterHelper::GetGameIconHandleForDxLib(const MdEntity& md_entity)
{
	return GetGameIconHandleForDxLib(md_entity.icon_id);
}

EBlendMode MasterHelper::GetAnimationBlendMode(const MdAnimation& animation)
{
	return MdSpriteSheet::Get(animation.sprite_id).blend_mode;
}

int MasterHelper::GetAnimationBlendModeForDxLib(const MdAnimation& animation)
{
	auto mode = GetAnimationBlendMode(animation);
	switch (mode)
	{
	case EBlendMode::Add:
		return DX_BLENDMODE_ADD;
	case EBlendMode::Alpha:
		return DX_BLENDMODE_ALPHA;
	case EBlendMode::Mula:
		return DX_BLENDMODE_MULA;
	}
	return DX_BLENDMODE_NOBLEND;
}

nlohmann::json MasterHelper::GetParticleJson(const MasterDataID particle_id)
{
	return GetParticleJson(MdParticle::Get(particle_id));
}

nlohmann::json MasterHelper::GetParticleJson(const MdParticle& md_particle)
{
	const std::string json_file_path = ResourcePaths::Dir::PARTICLES + md_particle.json_name;
	std::ifstream json_file(json_file_path);
	if (json_file)
	{
		nlohmann::json json = nlohmann::json::parse(json_file);
		return json;
	}

	throw std::runtime_error("Failed to open json file: " + json_file_path);
}
