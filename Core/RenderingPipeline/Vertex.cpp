#include "stdafx.h"
#include "Vertex.h"

namespace VertexCore
{
	VertexLayout::VertexInfo::VertexInfo(VertexType type, size_t offset) : type(type), offset(offset) { }

	size_t VertexLayout::VertexInfo::GetNextOffset() const NOEXCEPTRELEASE
	{
		return offset + size();
	}

	size_t VertexLayout::VertexInfo::GetOffset() const
	{
		return offset;
	}

	size_t VertexLayout::VertexInfo::size() const NOEXCEPTRELEASE
	{
		return GetTypeSize(type);
	}

	VertexLayout::VertexType VertexLayout::VertexInfo::GetType() const noexcept
	{
		return type;
	}

	D3D11_INPUT_ELEMENT_DESC VertexLayout::VertexInfo::GetInputDESC() const NOEXCEPTRELEASE
	{
		switch (type)
		{
		case VertexLayout::Position2D:
			return CreateInputDESC<Position2D>(GetOffset());

		case VertexLayout::Position3D:
			return CreateInputDESC<Position3D>(GetOffset());

		case VertexLayout::Texture2D:
			return CreateInputDESC<Texture2D>(GetOffset());

		case VertexLayout::Normal:
			return CreateInputDESC<Normal>(GetOffset());

		case VertexLayout::ColorFloat3:
			return CreateInputDESC<ColorFloat3>(GetOffset());

		case VertexLayout::ColorFloat4:
			return CreateInputDESC<ColorFloat4>(GetOffset());

		case VertexLayout::ARBGColor:
			return CreateInputDESC<ARBGColor>(GetOffset());
		}

		assert(false && "해당 Vertex 타입은 존재하지 않음");
		return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	const char* VertexLayout::VertexInfo::GetCode() const noexcept
	{
		switch (type)
		{
		case VertexCore::VertexLayout::Position2D:
			return Map<Position2D>::code;

		case VertexCore::VertexLayout::Position3D:
			return Map<Position3D>::code;

		case VertexCore::VertexLayout::Texture2D:
			return Map<Texture2D>::code;

		case VertexCore::VertexLayout::Normal:
			return Map<Normal>::code;

		case VertexCore::VertexLayout::ColorFloat3:
			return Map<ColorFloat3>::code;

		case VertexCore::VertexLayout::ColorFloat4:
			return Map<ColorFloat4>::code;

		case VertexCore::VertexLayout::ARBGColor:
			return Map<ARBGColor>::code;
		}

		assert("해당 Vertex Type은 존재하지 않습니다." && false);
		return "Invaild";
	}

	constexpr const size_t VertexLayout::VertexInfo::GetTypeSize(VertexType type) NOEXCEPTRELEASE
	{
		using namespace DirectX;

		switch (type)
		{
		case VertexLayout::Position2D:
			return sizeof(Map<Position2D>::Type);

		case VertexLayout::Position3D:
			return sizeof(Map<Position3D>::Type);

		case VertexLayout::Texture2D:
			return sizeof(Map<Texture2D>::Type);

		case VertexLayout::Normal:
			return sizeof(Map<Normal>::Type);

		case VertexLayout::ColorFloat3:
			return sizeof(Map<ColorFloat3>::Type);

		case VertexLayout::ColorFloat4:
			return sizeof(Map<ColorFloat4>::Type);

		case VertexLayout::ARBGColor:
			return sizeof(Map<ARBGColor>::Type);
		}

		assert("확인할 수 없는 Vertex Type" && false);

		return 0;
	}

	const VertexLayout::VertexInfo& VertexLayout::GetVertexInfoFromIndex(size_t i) const NOEXCEPTRELEASE
	{
		return vertexInfo[i];
	}

	VertexLayout& VertexLayout::AddType(VertexType type) NOEXCEPTRELEASE
	{
		vertexInfo.push_back({ type, size() });
		return *this;
	}

	size_t VertexLayout::size() const NOEXCEPTRELEASE
	{
		return vertexInfo.empty() ? 0 : vertexInfo.back().GetNextOffset();
	}

	size_t VertexLayout::count() const noexcept
	{
		return vertexInfo.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetInputElement() const NOEXCEPTRELEASE
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(count());

		for (const auto& e : vertexInfo)
			desc.push_back(e.GetInputDESC());

		return desc;
	}

	std::string VertexLayout::GetCode() const NOEXCEPTRELEASE
	{
		std::string code;

		for (const auto& info : vertexInfo)
			code += info.GetCode();

		return code;
	}

	Vertex::Vertex(char* data, const VertexLayout& vertexLayout) NOEXCEPTRELEASE : data(data), vertexLayout(vertexLayout)
	{
		assert(data != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& vertex) NOEXCEPTRELEASE : vertex(vertex) { }

	VertexBuffer::VertexBuffer(VertexLayout vertexLayout) NOEXCEPTRELEASE : vertexLayout(std::move(vertexLayout)) { }
	
	const char* VertexBuffer::data() const NOEXCEPTRELEASE
	{
		return buffer.data();
	}

	const VertexLayout& VertexBuffer::GetVertexLayout() const noexcept
	{
		return vertexLayout;
	}

	size_t VertexBuffer::count() const NOEXCEPTRELEASE
	{
		return buffer.size() / vertexLayout.size();
	}

	size_t VertexBuffer::size() const NOEXCEPTRELEASE
	{
		return buffer.size();
	}

	Vertex VertexBuffer::back() NOEXCEPTRELEASE
	{
		assert("정점 타입 정보가 존재하지 않습니다." && (buffer.size() != 0));

		// 쓰래기 값이 있더라도 그 값을 반환 함
		return Vertex{ buffer.data() + buffer.size() - vertexLayout.size(), vertexLayout };
	}

	Vertex VertexBuffer::front() NOEXCEPTRELEASE
	{
		assert("정점 타입 정보가 존재하지 않습니다." && (buffer.size() != 0));
		return Vertex{ buffer.data(), vertexLayout };
	}

	Vertex VertexBuffer::operator[](size_t i) NOEXCEPTRELEASE
	{
		assert("정점 타입 정보 배열 길이를 넘었습니다." && (i < count()));
		return Vertex{ buffer.data() + vertexLayout.size() * i, vertexLayout };
	}

	ConstVertex VertexBuffer::back() const NOEXCEPTRELEASE
	{
		return const_cast<VertexBuffer*>(this)->back();
	}

	ConstVertex VertexBuffer::front() const NOEXCEPTRELEASE
	{
		return const_cast<VertexBuffer*>(this)->front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const NOEXCEPTRELEASE
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}