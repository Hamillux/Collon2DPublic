#pragma once
#include "MasterData.h"
#include <imgui.h>
#include "Utility/ImGui/internal/ImGuiExtensions.h"
namespace MasterHelper {

/// <summary>
/// IconIDによってアイコンのImTextureID, uv0, uv1を取得する
/// </summary>
void GetGameIconImguiImage(const MasterDataID icon_id, ImTextureID& out_im_tex_id, float& out_u0, float& out_v0, float& out_u1, float& out_v1);
void GetGameIconImguiImage(const MasterDataID icon_id, ImGui::Texture& texture);

// NOTE: 取得したハンドルの管理はクライアントが行う
int GetGameIconHandleForDxLib(const MasterDataID icon_id);

// NOTE: 取得したハンドルの管理はクライアントが行う
int GetGameIconHandleForDxLib(const MdItem& md_item);

// NOTE: 取得したハンドルの管理はクライアントが行う
int GetGameIconHandleForDxLib(const MdEntity& md_entity);

EBlendMode GetAnimationBlendMode(const MdAnimation& animation);
int GetAnimationBlendModeForDxLib(const MdAnimation& animation);

nlohmann::json GetParticleJson(const MasterDataID particle_id);
nlohmann::json GetParticleJson(const MdParticle& md_particle);

/// <summary>
/// image_idをメンバに持つクラスから画像ファイルのマスターデータを取得する
/// </summary>
template<typename T>
inline const MdImageFile& GetImageFile(const T& reffering_to_image)
{
	return MdImageFile::Get(reffering_to_image.image_id);
}

}// end namespace 'MasterHelper'