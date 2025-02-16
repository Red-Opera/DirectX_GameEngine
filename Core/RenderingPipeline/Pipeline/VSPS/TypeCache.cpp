#include "stdafx.h"
#include "TypeCache.h"

namespace DynamicConstantBuffer
{
	CompleteLayout TypeCache::Get(EditLayout&& layout) NOEXCEPTRELEASE
	{
		auto signature = layout.GetSignature();
		auto& typeMap = get_().cacheData;
		const auto i = typeMap.find(signature);

		if (i != typeMap.end())
		{
			layout.ClearLayout();
			return { i->second };
		}

		auto result = typeMap.insert({ std::move(signature), layout.GetLayout() });

		return { result.first->second };
	}

	TypeCache& TypeCache::get_()
	{
		static TypeCache cache;

		return cache;
	}
}