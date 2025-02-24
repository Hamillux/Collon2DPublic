#include "GraphResourceManager.h"
#include <windows.h>
#include <DirectXTex.h>
#include <DxLib.h>
#include "Utility/Core/StringUtils.h"

GraphicResourceManager::GraphicResourceManager()
{
}

void GraphicResourceManager::UnloadSprite(const MasterDataID sprite_id)
{
    if (!IsLoadedSprite(sprite_id))
    {
        return;
    }

    std::vector<int>& handles = _loaded_sprites.at(sprite_id);
    for (int handle : handles)
    {
        DxLib::DeleteGraph(handle);
    }
    _loaded_sprites.erase(sprite_id);
}

void GraphicResourceManager::UnloadGraphForDxLib(const std::string& file_path)
{
    if (!IsLoadedGraphForDxLib(file_path))
    {
        return;
    }

    const int handle = _loaded_graphs_for_dxlib.at(file_path);
    DxLib::DeleteGraph(handle);
    _loaded_graphs_for_dxlib.erase(file_path);
}

bool GraphicResourceManager::IsLoadedGraphForDxLib(const std::string& file_path) const
{
    return _loaded_graphs_for_dxlib.find(file_path) != _loaded_graphs_for_dxlib.end();
}

bool GraphicResourceManager::IsLoadedGraphForDxLib(const MasterDataID image_id) const
{
    const std::string& file_path = MdImageFile::GetPath(image_id);
    return IsLoadedGraphForDxLib(file_path);
}

bool GraphicResourceManager::IsLoadedSprite(const MasterDataID sprite_id) const
{
    return _loaded_sprites.find(sprite_id) != _loaded_sprites.end();
}

bool GraphicResourceManager::IsLoadedTexture(const MasterDataID image_id) const
{
    return _loaded_textures.find(image_id) != _loaded_textures.end();
}

GraphicResourceManager::~GraphicResourceManager()
{
}

void GraphicResourceManager::Finalize()
{
    UnloadAll();
    __super::Finalize();
}

void GraphicResourceManager::UnloadAll()
{
    if (_loaded_textures.size() > 0)
    {
        _loaded_textures.clear();
    }

    if (_loaded_graphs_for_dxlib.size() > 0)
    {
        for (auto& pair : _loaded_graphs_for_dxlib)
        {
            int ghandle = pair.second;
            DxLib::DeleteGraph(ghandle);
        }
        _loaded_graphs_for_dxlib.clear();
    }

    if (_loaded_sprites.size() > 0)
    {
        for (auto& pair : _loaded_sprites)
        {
            for (auto& ghandle : pair.second)
            {
                DxLib::DeleteGraph(ghandle);
            }
        }
        _loaded_sprites.clear();
    }
}

int GraphicResourceManager::LoadGraphForDxLib(const std::string& file_path)
{
    // ロード済みの場合は削除して再ロード
    if (IsLoadedGraphForDxLib(file_path))
    {
        UnloadGraphForDxLib(file_path);
    }

    const int ghandle = DxLib::LoadGraph(to_tstring(file_path).c_str());
    if (ghandle == -1)
    {
        throw std::runtime_error("Failed to load graph");
    }

    _loaded_graphs_for_dxlib[file_path] = ghandle;
    return ghandle;
}

int GraphicResourceManager::LoadGraphForDxLib(const MasterDataID image_id)
{
    const std::string& file_path = MdImageFile::GetPath(image_id);
    return LoadGraphForDxLib(file_path);
}

void GraphicResourceManager::LoadTexture(const MasterDataID image_id, ID3D11Resource** out_pp_resource, ID3D11ShaderResourceView** out_pp_srv)
{
    // ロード済みの場合は削除して再ロード
    if (IsLoadedTexture(image_id))
    {
        _loaded_textures.erase(image_id);
    }

    // デバイスの取得
    auto p_dev = const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(DxLib::GetUseDirect3D11Device()));
    Texture new_texture{};

    DirectX::ScratchImage scratch_image;
    HRESULT hr;
    const std::wstring path_wstr = StringToWString(MdImageFile::GetPath(image_id));

    // 画像ファイルのロード
    const EFileExtension ext = GetFileExtension(path_wstr);
    switch (ext)
    {
    case EFileExtension::DDS:
        hr = DirectX::LoadFromDDSFile(path_wstr.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scratch_image);
        if (FAILED(hr))
        {
            throw std::runtime_error("[GraphicResourceManager::LoadTexture()]failed to load DDS file");
        }
        break;
    case EFileExtension::PNG:
        hr = DirectX::LoadFromWICFile(path_wstr.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch_image);
        if (FAILED(hr))
        {
            throw std::runtime_error("[GraphicResourceManager::LoadTexture()]failed to load PNG file");
        }
        break;
    case EFileExtension::BMP:
        hr = DirectX::LoadFromWICFile(path_wstr.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch_image);
        if (FAILED(hr))
        {
            throw std::runtime_error("[GraphicResourceManager::LoadTexture()]failed to load BMP file");
        }
        break;
    default:
        throw std::runtime_error("[GraphicResourceManager::LoadTexture()]unsupported file extension");
        break;
    }

    // リソースの作成
    hr = DirectX::CreateTexture(p_dev, scratch_image.GetImages(), scratch_image.GetImageCount(), scratch_image.GetMetadata(), new_texture.p_resource.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("[GraphicResourceManager::LoadTexture()]failed to create texture");
    }

    // SRVの作成
    hr = DirectX::CreateShaderResourceView(p_dev, scratch_image.GetImages(), scratch_image.GetImageCount(), scratch_image.GetMetadata(), new_texture.p_srv.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("[GraphicResourceManager::LoadTexture()]failed to create SRV");
    }

    // マップに追加
    _loaded_textures[image_id] = new_texture;

    if (out_pp_resource)
    {
        *out_pp_resource = new_texture.p_resource.Get();
    }
    if (out_pp_srv)
    {
        *out_pp_srv = new_texture.p_srv.Get();
    }
}

void GraphicResourceManager::LoadSpriteForDxLib(const MasterDataID sprite_id, std::vector<int>* out_sprite_handles)
{
    // ロード済みの場合は削除して再ロード
    if (IsLoadedSprite(sprite_id))
    {
        UnloadSprite(sprite_id);
    }

    const MdSpriteSheet& sprite_sheet = MdSpriteSheet::Get(sprite_id);
    const MdImageFile& image = MdImageFile::Get(sprite_sheet.image_id);
    const int num_x = sprite_sheet.num_columns;
    const int num_y = sprite_sheet.num_rows;
    const int size_x = image.width / num_x;
    const int size_y = image.height / num_y;

    _loaded_sprites[sprite_id] = std::vector<int>(num_x * num_y);
    std::vector<int>& new_handles = _loaded_sprites.at(sprite_id);

    DxLib::LoadDivGraph(to_tstring(image.path).c_str(), new_handles.size(), num_x, num_y, size_x, size_y, new_handles.data());

    if (out_sprite_handles)
    {
        *out_sprite_handles = new_handles;
    }
}

int GraphicResourceManager::GetGraphForDxLib(const MasterDataID image_id)
{
    const std::string& file_path = MdImageFile::GetPath(image_id);
    return GetGraphForDxLib(file_path);
}

int GraphicResourceManager::GetGraphForDxLib(const std::string& file_path)
{
    if (!IsLoadedGraphForDxLib(file_path))
    {
        return LoadGraphForDxLib(file_path);
    }

    return _loaded_graphs_for_dxlib.at(file_path);
}

void GraphicResourceManager::GetTexture(const MasterDataID image_id, ID3D11Resource** out_p_resource, ID3D11ShaderResourceView** out_p_srv)
{
    if (!IsLoadedTexture(image_id))
    {
        LoadTexture(image_id, out_p_resource, out_p_srv);
        return;
    }

    const Texture& tex = _loaded_textures.at(image_id);

    if (out_p_resource)
    {
        *out_p_resource = tex.p_resource.Get();
    }
    if (out_p_srv)
    {
        *out_p_srv = tex.p_srv.Get();
    }
}

const std::vector<int>& GraphicResourceManager::GetSprite(const MasterDataID sprite_id)
{
    if (!IsLoadedSprite(sprite_id))
    {
        LoadSpriteForDxLib(sprite_id);
    }

    return _loaded_sprites.at(sprite_id);
}

int GraphicResourceManager::GetDerivedGraph(const MasterDataID image_id, const int left, const int top, const int width, const int height)
{
    const int ghandle_parent = GetGraphForDxLib(image_id);
    return DxLib::DerivationGraph(left, top, width, height, ghandle_parent);
}
