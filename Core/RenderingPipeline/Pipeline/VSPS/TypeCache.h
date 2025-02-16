#pragma once

#include "DynamicConstantBuffer.h"

#include <string>
#include <memory>
#include <unordered_map>

// 레이아웃의 타입을 저장하는 클래스
namespace DynamicConstantBuffer
{
	class TypeCache
	{
	public:
		static CompleteLayout Get(EditLayout&& layout) NOEXCEPTRELEASE;

	private:
		static TypeCache& get_();

		std::unordered_map<std::string, std::shared_ptr<LayoutType>> cacheData;
	};
}