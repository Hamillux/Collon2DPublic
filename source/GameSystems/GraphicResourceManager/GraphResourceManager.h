#pragma once
#include <unordered_map>
#include <d3d11.h>
#include <wrl/client.h>
#include "Utility/SingletonBase.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

/// <summary>
/// 画像リソースの管理クラス
/// </summary>
class GraphicResourceManager : public Singleton<GraphicResourceManager>
{
private:
	friend class Singleton<GraphicResourceManager>;
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	virtual ~GraphicResourceManager();

	virtual void Finalize() override;

	void UnloadAll();

	/// <summary>
	/// DxLib用に画像をロードする. ロード済みの場合は再ロードする
	/// </summary>
	/// <param name="file_path">画像パス</param>
	/// <returns>DxLibグラフィックハンドル</returns>
	int LoadGraphForDxLib(const std::string& file_path);

	/// <summary>
	/// DxLib用に画像をロードする. ロード済みの場合は再ロードする
	/// </summary>
	/// <param name="image_id">画像ID</param>
	/// <returns>DxLibグラフィックハンドル</returns>
	int LoadGraphForDxLib(const MasterDataID image_id);

	/// <summary>
	/// DxLib用にスプライトをロードする. ロード済みの場合は再ロードする
	/// </summary>
	/// <param name="sprite_id"></param>
	/// <param name="out_sprite_handles"></param>
	void LoadSpriteForDxLib(const MasterDataID sprite_id, std::vector<int>* out_sprite_handles = nullptr);

	void LoadTexture(const MasterDataID image_id, ID3D11Resource** out_pp_resource = nullptr, ID3D11ShaderResourceView** out_pp_srv = nullptr);

	// 未ロードの場合, ロードして返却
	int GetGraphForDxLib(const MasterDataID image_id);
	int GetGraphForDxLib(const std::string& file_path);
	void GetTexture(const MasterDataID image_id, ID3D11Resource** out_pp_resource, ID3D11ShaderResourceView** out_pp_srv);
	const std::vector<int>& GetSprite(const MasterDataID sprite_id);

	// この関数で取得したハンドルはGraphResourceManagerの管理対象外となるので, 呼び出し元でDxLib::DeleteGraphを使って破棄する
	int GetDerivedGraph(const MasterDataID image_id, const int left, const int top, const int width, const int height);

private:
	GraphicResourceManager();

	void UnloadSprite(const MasterDataID sprite_id);
	void UnloadGraphForDxLib(const std::string& file_path);

	bool IsLoadedGraphForDxLib(const std::string& file_path) const;
	bool IsLoadedGraphForDxLib(const MasterDataID image_id) const;
	bool IsLoadedSprite(const MasterDataID sprite_id) const;
	bool IsLoadedTexture(const MasterDataID image_id) const;

	struct Texture
	{
		Texture() {}

		Texture(ID3D11Resource* in_resource, ID3D11ShaderResourceView* in_srv)
			: p_resource(in_resource)
			, p_srv(in_srv)
		{}

		bool IsValid() const
		{
			return (
				p_resource != nullptr &&
				p_srv != nullptr);
		}

		ComPtr<ID3D11Resource> p_resource;			// テクスチャリソース
		ComPtr<ID3D11ShaderResourceView> p_srv;		// ViewDimensionがTEXTURE2DのSRV
	};

	std::unordered_map <MasterDataID, Texture> _loaded_textures;		// キーは画像ID
	std::unordered_map<std::string, int>  _loaded_graphs_for_dxlib;		// 値はDxLibグラフィックハンドル
	std::unordered_map<MasterDataID, std::vector<int>> _loaded_sprites;
};