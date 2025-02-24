#include "RandomNumberGenerator.h"
#include <random>

std::mt19937* RandomNumberGenerator::_generator_32 = nullptr;
constexpr unsigned int rand_max_32 = std::mt19937::max();
constexpr unsigned int rand_min_32 = std::mt19937::min();

std::mt19937_64* RandomNumberGenerator::_generator_64 = nullptr;
constexpr uint64_t rand_max_64 = std::mt19937_64::max();
constexpr uint64_t rand_min_64 = std::mt19937_64::min();


float RandomNumberGenerator::GetRandomFloat(float min, float max)
{
	if (!_generator_32)
	{
		CreateGenerators();
	}
	return (min + GetNormalizedRandomValue() * (max - min));
}

uint64_t RandomNumberGenerator::GetRandomUint64()
{
	return (*_generator_64)();
}

void RandomNumberGenerator::GenerateUUID(uint64_t& high_bits, uint64_t& low_bits)
{
	if (!_generator_64)
	{
		CreateGenerators();
	}

	high_bits = (*_generator_64)();
	low_bits = (*_generator_64)();

	// RFC4122のUUID_v4はXXXXXXXX-XXXX-4XXX-NXXX-XXXXXXXXXXXX (Nは2進表記で10XX)
	// high_bits: XXXXXXXX-XXXX-4XXX
	// low_bits : NXXX-XXXXXXXXXXXX
	high_bits = (high_bits & 0xFFFF'FFFF'FFFF'0FFF) | 0x0000'0000'0000'4000;
	low_bits = (low_bits & 0x3FFF'FFFF'FFFF'FFFF) | 0x8000'0000'0000'0000;
}

bool RandomNumberGenerator::ReturnTrueWithProbability(const float p)
{
	if (p == 1.f)
	{
		return true;
	}
	else if (p == 0.f)
	{
		return false;
	}

	return GetNormalizedRandomValue() < p;
}

void RandomNumberGenerator::CreateGenerators()
{
	std::random_device rd;
	_generator_32 = new std::mt19937(rd());
	_generator_64 = new std::mt19937_64(rd());
}

double RandomNumberGenerator::GetNormalizedRandomValue()
{
	if (!_generator_32)
	{
		CreateGenerators();
	}
	return static_cast<double>((*_generator_32)()) / rand_max_32;
}
