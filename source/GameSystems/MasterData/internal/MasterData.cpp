#include "MasterData.h"
#include "SystemTypes.h"
#include "Utility/Core/CodeGenerationHelper.h"

const char* CsvFileName<MdAnimation>::value = "collon2D_md_Animation.csv";
const char* CsvFileName<MdGameIcon>::value = "collon2D_md_GameIcon.csv";
const char* CsvFileName<MdImageFile>::value = "collon2D_md_ImageFile.csv";
const char* CsvFileName<MdBlockSkin>::value = "collon2D_md_BlockSkin.csv";
const char* CsvFileName<MdEntity>::value = "collon2D_md_Entity.csv";
const char* CsvFileName<MdSpriteSheet>::value = "collon2D_md_SpriteSheet.csv";
const char* CsvFileName<MdParticle>::value = "collon2D_md_Particle.csv";
const char* CsvFileName<MdFont>::value = "collon2D_md_Font.csv";
const char* CsvFileName<MdItem>::value = "collon2D_md_Item.csv";
const char* CsvFileName<MdStageBGM>::value = "collon2D_md_StageBgm.csv";
// TODO: マスターデータの種類を増やす場合, ここに追加


namespace
{
	template<typename T>
	void LoadMasterDataFromCSVImpl()
	{
		std::string path = std::string(ResourcePaths::Dir::MASTER_DATA) + std::string(CsvFileName<T>::value);
		T::LoadFromCSV(path);
	}

	template<typename Head, typename...Tails>
	void LoadMasterDataFromCSV()
	{
		LoadMasterDataFromCSVImpl<Head>();
		if constexpr (sizeof...(Tails))
		{
			LoadMasterDataFromCSV<Tails...>();
		}
	}

} // end anonymous-namespace

void LoadAllMasterData()
{
	static bool has_loaded = false;
	if (has_loaded)
	{
		return;
	}

	LoadMasterDataFromCSV<
		MdAnimation,
		MdGameIcon,
		MdImageFile,
		MdBlockSkin,
		MdEntity,
		MdSpriteSheet,
		MdParticle,
		MdFont,
		MdItem,
		MdStageBGM
		// TODO: マスターデータの追加
	>();

	has_loaded = true;
}