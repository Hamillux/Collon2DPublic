#include "MdEntity.h"

void MdEntity::CountEntitiesInCategory(std::unordered_map<EEntityCategory, size_t>& out_counts)
{
	for (auto& e : EnumInfo<EEntityCategory>::List())
	{
		out_counts[e] = 0;
	}
	for (auto& data : GetData())
	{
		++out_counts[EnumInfo<EEntityCategory>::StringToEnum(data.category_str)];
	}
}
