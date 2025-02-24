#pragma once
#include <unordered_map>
#include <d3d11.h>
#include <wrl/client.h>
#include "Utility/SingletonBase.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

class GraphicResourceManager : public Singleton<GraphicResourceManager>
{
private:
	friend class Singleton<GraphicResourceManager>;
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	virtual ~GraphicResourceManager();

	virtual void Finalize() override;

	void UnloadAll();

	// ロード済みの場合, 削除して再ロード
	int LoadGraphForDxLib(const std::string& file_path);
	int LoadGraphForDxLib(const MasterDataID image_id);
	void LoadTexture(const MasterDataID image_id, ID3D11Resource** out_pp_resource = nullptr, ID3D11ShaderResourceView** out_pp_srv = nullptr);
	void LoadSprite(const MasterDataID sprite_id);

	// 未ロードの場合, ロードして返却
	int GetGraphForDxLib(const MasterDataID image_id);
	int GetGraphForDxLib(const std::string& file_path);
	void GetTexture(const MasterDataID image_id, ID3D11Resource** out_pp_resource, ID3D11ShaderResourceView** out_pp_srv);
	const std::vector<int>& GetSprite(const MasterDataID sprite_id);

	// この関数で取得したハンドルはGraphResourceManagerの管理対象外となるので, 呼び出し元でDxLib::DeleteGraphを使って破棄する
	int GetDerivedGraph(const MasterDataID image_id, const int left, const int top, const int width, const int height);

private:
	GraphicResourceManager();

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

	std::unordered_map <MasterDataID, Texture> _loaded_textures;		// 値はDX11のリソース, SRV
	std::unordered_map<std::string, int>  _loaded_graphs_for_dxlib;		// 値はDxLibグラフィックハンドル
	std::unordered_map<MasterDataID, std::vector<int>> _loaded_sprites;
};