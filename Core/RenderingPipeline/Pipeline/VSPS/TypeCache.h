#pragma once

#include "DynamicConstantBuffer.h"

#include <string>
#include <memory>
#include <unordered_map>

// ���̾ƿ��� Ÿ���� �����ϴ� Ŭ����
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