#pragma once
#include <random>

class RandomNumberGenerator
{
public:
	/// <summary>
	/// [min,max]のランダムな値を取得
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	static float GetRandomFloat(float min, float max);
	static uint64_t GetRandomUint64();

	/// <summary>
	/// UUID(128bit)を生成
	/// <para>生成するUUIDはRFC4122のUUID(v4):</para>
	/// <para>XXXXXXXX-XXXX-4XXX-NXXX-XXXXXXXXXXXX (Nは2進表記で10XX)</para>
	/// </summary>
	/// <param name="high_bits">上位64bit</param>
	/// <param name="low_bits">下位64bit</param>
	static void GenerateUUID(uint64_t& high_bits, uint64_t& low_bits);

	/// <summary>
	/// 指定された確率でtrueを返す関数
	/// </summary>
	/// <param name="p">確率を表す[0,1]の値</param>
	static bool ReturnTrueWithProbability(const float p);

private:
	static void CreateGenerators();

	/// <summary>
	/// [0,1]のランダムな値を生成
	/// </summary>
	/// <returns></returns>
	static double GetNormalizedRandomValue();
	static std::mt19937* _generator_32;
	static std::mt19937_64* _generator_64;
};