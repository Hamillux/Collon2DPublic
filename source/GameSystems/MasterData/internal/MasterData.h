#pragma once
#include "MasterDataBase.h"
#include "MdGameIcon.h"
#include "MdAnimation.h"
#include "MdImageFile.h"
#include "MdBlockSkin.h"
#include "MdEntity.h"
#include "MdSpriteSheet.h"
#include "MdParticle.h"
#include "MdFont.h"
#include "MdItem.h"
#include "MdStageBGM.h"

// マスターデータが定義されたCSVファイルの名前.
// NOTE: 特殊化にはstatic const char* valueを宣言し, MasterData.cppで定義する
template<class MasterDataType>
struct CsvFileName
{};

// CsvFileNameテンプレートの特殊化
template<> struct CsvFileName<MdAnimation> { static const char* value; };
template<> struct CsvFileName<MdGameIcon> { static const char* value; };
template<> struct CsvFileName<MdImageFile> { static const char* value; };
template<> struct CsvFileName<MdBlockSkin> { static const char* value; };
template<> struct CsvFileName<MdEntity> { static const char* value; };
template<> struct CsvFileName<MdSpriteSheet> { static const char* value; };
template<> struct CsvFileName<MdParticle> { static const char* value; };
template<> struct CsvFileName<MdFont> { static const char* value; };
template<> struct CsvFileName<MdItem> { static const char* value; };
template<> struct CsvFileName<MdStageBGM> { static const char* value; };
// TODO: マスターデータの種類を増やす場合, ここに追加

/// <summary>
/// ゲーム起動 から ゲーム終了 までの間に1度だけロードする
/// </summary>
void LoadAllMasterData();