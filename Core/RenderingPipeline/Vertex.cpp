#define USE_VERTEX_CODE

#include "stdafx.h"
#include "Vertex.h"

namespace VertexCore
{
	template<VertexLayout::VertexType type>
	struct SystemSize
	{
		static constexpr auto Run() noexcept { return sizeof(VertexLayout::Map<type>::Type); }
	};

	template<VertexLayout::VertexType type>
	struct GetTypeCode
	{
		static constexpr auto Run() noexcept { return VertexLayout::Map<type>::code; }
	};

	// Input Layout DESC를 만들어주는 메소드
	template<VertexLayout::VertexType type>
	struct CreateInputDESC
	{
		static constexpr D3D11_INPUT_ELEMENT_DESC Run(size_t offset) noexcept
		{
			return 
			{
				VertexLayout::Map<type>::semantic, 
				0, 
				VertexLayout::Map<type>::dxgiFormat, 
				0, 
				(UINT)offset, 
				D3D11_INPUT_PER_VERTEX_DATA, 
				0 
			};
		}
	};

	VertexLayout::VertexInfo::VertexInfo(VertexType type, size_t offset) : type(type), offset(offset) { }

	constexpr const size_t VertexLayout::VertexInfo::GetTypeSize(VertexType type) NOEXCEPTRELEASE
	{
		return RunStructFunction<SystemSize>(type);
	}

	const char* VertexLayout::VertexInfo::GetCode() const noexcept
	{
		return RunStructFunction<GetTypeCode>(type);
	}

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
		return RunStructFunction<CreateInputDESC>(type, GetOffset());
	}

	const VertexLayout::VertexInfo& VertexLayout::GetVertexInfoFromIndex(size_t i) const NOEXCEPTRELEASE
	{
		return vertexInfo[i];
	}

	VertexLayout& VertexLayout::AddType(VertexType type) NOEXCEPTRELEASE
	{
		if (!has(type))
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

	bool VertexLayout::has(VertexType type) const noexcept
	{
		for (auto& info : vertexInfo)
		{
			if (info.GetType() == type)
				return true;
		}

		return false;
	}

	Vertex::Vertex(char* data, const VertexLayout& vertexLayout) NOEXCEPTRELEASE : data(data), vertexLayout(vertexLayout)
	{
		assert(data != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& vertex) NOEXCEPTRELEASE : vertex(vertex) { }

	VertexBuffer::VertexBuffer(VertexLayout vertexLayout, size_t size) NOEXCEPTRELEASE 
		: vertexLayout(std::move(vertexLayout)) 
	{ 
		Resize(size);
	}
	template<VertexLayout::VertexType type>
	struct VertexSettingFromAIMEsh
	{
		static constexpr void Run(VertexBuffer* vertexBuffer, const aiMesh& mesh) NOEXCEPTRELEASE
		{
			for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
				(*vertexBuffer)[i].GetValue<type>() = VertexLayout::Map<type>::get(mesh, i);
		}
	};

	VertexBuffer::VertexBuffer(VertexLayout vertexLayout, const aiMesh& mesh) 
		: vertexLayout(std::move(vertexLayout))
	{
		Resize(mesh.mNumVertices);

		for (size_t i = 0, end = this->vertexLayout.count(); i < end; i++)
			VertexLayout::RunStructFunction<VertexSettingFromAIMEsh>
			(
				this->vertexLayout.GetVertexInfoFromIndex(i).GetType(), 
				this, 
				mesh
			);
	}
	
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

	void VertexBuffer::Resize(size_t newSize) NOEXCEPTRELEASE
	{
		const auto currentSize = size();

		if (currentSize < newSize)
			buffer.resize(buffer.size() + vertexLayout.size() * (newSize - currentSize));
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