#pragma once
#include "Utility/Core/FileUtil.h"
#include <tchar.h>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>

/// <summary>
/// マスターデータIDの型
/// </summary>
using MasterDataID = unsigned long;
constexpr MasterDataID INVALID_MASTER_ID = 0;

// CRTP基底クラス. 
// 派生クラスにはメンバ関数GetMembers(), GetMapKey()の実装が必要
// std::tuple<T0&, T1&, T2& ...> GetMembers()
// -> CSVのカラムに対応するメンバをm0, m1, m2, ...としてreturn std::tie(m0, m1, m2, ...);
// MasterDataID GetMapKey() const
// -> 検索に使用するキーを返す. 通常はidメンバーを返す
template<class T>
class MasterData
{
	//~ Begin MasterData interface
public:
	virtual bool IsValid() const
	{
		return true;
	}
	//~ End MasterData interface

public:
	static void LoadFromCSV(const std::string& csv_path)
	{
		std::ifstream file(csv_path);
		if (!file.is_open())
		{
			throw std::runtime_error("Cannot open file: " + csv_path);
		}

		const size_t num_lines = CountLines(csv_path);
		_loaded_data = std::vector<T>();
		_loaded_data.reserve(num_lines);

		// _loaded_dataの構築
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream ss(line);
			try
			{
				T instance = {};
				LoadFromCsvRow(ss, instance.GetMembers());
				if (!instance.IsValid())
				{
					throw std::runtime_error("Detected invalid data");
				}
				_loaded_data.push_back(instance);
			}
			catch (std::exception&)
			{
				continue;
			}
		}

		// _loaded_dataをGetMapKey()の値でソート
		std::sort(_loaded_data.begin(), _loaded_data.end(), [](const T& a, const T& b) { return a.GetMapKey() < b.GetMapKey(); });

		// _data_mapの構築
		for (size_t i = 0; i < _loaded_data.size(); i++)
		{
			const MasterDataID key = _loaded_data.at(i).GetMapKey();
			if (_data_map.find(key) != _data_map.end())
			{
				throw std::runtime_error("Duplicate id: " + std::to_string(key));
			}
			_data_map[key] = i;
		}
	}

	/// <summary>
	/// マスターデータIDとデータリストのインデックスのマップを取得
	/// </summary>
	static const auto& GetMap()
	{
		return _data_map;
	}

	/// <summary>
	/// マスターデータのリストを取得
	/// </summary>
	/// <returns></returns>
	static const std::vector<T>& GetData()
	{
		return _loaded_data;
	}

	/// <summary>
	/// キーによるデータの取得. 検索には事前に構築されたマップを使用するのでO(1)
	/// </summary>
	/// <param name="key">検索に使用するキー</param>
	/// <returns>GetMapKey() == key となるデータ</returns>
	static const T& Get(const MasterDataID key)
	{
		return _loaded_data.at(_data_map.at(key));
	}

	/// <summary>
	/// predicateを満たすデータのIDを取得. O(N)
	/// </summary>
	/// <param name="pred">検索条件</param>
	/// <returns>最初に見つかったID. 見つからなかった場合はINVALID_MASTER_ID</returns>
	template<class Predicate>
	static MasterDataID Find(Predicate pred)
	{
		typename std::vector<T>::iterator target = std::find_if(_loaded_data.begin(), _loaded_data.end(), pred);
		if (target == _loaded_data.end())
		{
			return INVALID_MASTER_ID;
		}

		const size_t index_of_target = target - _loaded_data.begin();

		const MasterDataID target_id = _data_map.find(index_of_target)->first;
		return target_id;
	}

	/// <summary>
	/// 指定のキーが有効なキーの中で何番目(0,1,...)にあるかを取得.
	/// 事前に構築されたマップを使用するのでO(1)
	/// </summary>
	/// <param name="id">GetMapKey()で取得するキー</param>
	static size_t GetIndex(const MasterDataID id)
	{
		return _data_map.at(id);
	}

	bool operator==(const MasterData<T>& other) const
	{
		return this->GetMapKey() == other.GetMapKey();
	}

private:
	/// <summary>
	/// GetMapKey()の値でソートされる
	/// </summary>
	static std::vector<T> _loaded_data;

	/// <summary>
	/// データの検索に使用するマップ
	/// <para>キー: GetMapKey()で取得する</para>
	/// <para>値  : _loaded_dataのインデックス</para>
	/// </summary>
	static std::unordered_map<MasterDataID, size_t> _data_map;
};
template<class T>
std::vector<T> MasterData<T>::_loaded_data = std::vector<T>();
template<class T>
std::unordered_map<MasterDataID, size_t> MasterData<T>::_data_map = std::unordered_map<MasterDataID, size_t>();
