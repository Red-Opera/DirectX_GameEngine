#pragma once

#include <vector>
#include <type_traits>

#include <DirectXMath.h>
#include <minwindef.h>

#include "Core/DxGraphic.h"
#include "Core/Draw/Base/ColorInfo.h"

namespace VertexCore
{
	class VertexLayout
	{
	public:
		enum VertexType { Position2D, Position3D, Texture2D, Normal, Tangent, BiTangent,ColorFloat3, ColorFloat4, ARBGColor, Count };

		template<VertexType> struct Map;
		template<> struct Map<Position2D>
		{
			using Type = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;

			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "Position2D";
		};

		template<> struct Map<Position3D>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "Position3D";
		};

		template<> struct Map<Texture2D>
		{
			using Type = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;

			static constexpr const char* semantic = "TexCoord";
			static constexpr const char* code = "Texture2D";
		};

		template<> struct Map<Normal>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "Normal";
		};

		template<> struct Map<Tangent>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Tangent";
		};

		template<> struct Map<BiTangent>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "BiTangent";
		};

		template<> struct Map<ColorFloat3>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color3D";
		};

		template<> struct Map<ColorFloat4>
		{
			using Type = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color4D";
		};

		template<> struct Map<ARBGColor>
		{
			using Type = ::ARBGColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color4D8Bit";
		};

		class VertexInfo
		{
		public:
			VertexInfo(VertexType type, size_t offset);

			size_t GetNextOffset() const NOEXCEPTRELEASE;					// 이 데이터 포함했을 때 다음 데이터 위치를 나타내는 메소드
			size_t GetOffset() const;										// 이 데이터가 시작하는 위치
			size_t size() const NOEXCEPTRELEASE;							// 이 데이터 타입의 크기

			VertexType GetType() const noexcept;							// 타입 정보를 가져오는 메소드
			D3D11_INPUT_ELEMENT_DESC GetInputDESC() const NOEXCEPTRELEASE;	// Input Layout의 DESC를 가져오는 메소드

			const char* GetCode() const noexcept;

			// 입력된 VertexType에 다라 실제 메모리 크기를 컴파일 타임에 반환해주는 함수
			static const constexpr size_t GetTypeSize(VertexType type) NOEXCEPTRELEASE;

		private:
			// Input Layout DESC를 만들어주는 메소드
			template<VertexType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC CreateInputDESC(size_t offset) noexcept
			{
				return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

			VertexType type;	// 정점 타입
			size_t offset;		// 해당 정점 타입의 시작 메모리 위치
		};

		// 해장 정점 타입이 저장되어 있는 정보를 가져오는 메소드
		template<VertexType Type>
		const VertexInfo GetVertexInfo() const NOEXCEPTRELEASE
		{
			// 해당 타입의 정로
			for (auto& info : vertexInfo)
			{
				if (info.GetType() == Type)
					return info;
			}

			assert("등록된 정점 정보가 존재하지 않습니다." && false);

			return vertexInfo.front();
		}

		const VertexInfo& GetVertexInfoFromIndex(size_t i) const NOEXCEPTRELEASE;	// 인덱스를 통해서 정점 타입을 가져오는 메소드
		VertexLayout& AddType(VertexType type) NOEXCEPTRELEASE;						// 정점 타입에 해당 타입의 정보를 추가하는 메소드

		// 정점 타입들의 총 크기, 정점 개수
		size_t size() const NOEXCEPTRELEASE;
		size_t count() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElement() const NOEXCEPTRELEASE;
		std::string GetCode() const NOEXCEPTRELEASE;

	private:
		std::vector<VertexInfo> vertexInfo;
	};

	class Vertex
	{
		friend class VertexBuffer;

	public:
		// 해당 타입에 맞는 Vertex 정보에서 Type에 맞는 값을 반환함
		template<VertexLayout::VertexType Type>
		auto& GetValue() NOEXCEPTRELEASE
		{
			auto value = data + vertexLayout.GetVertexInfo<Type>().GetOffset();

			return *reinterpret_cast<typename VertexLayout::Map<Type>::Type*>(value);
		}

		template<typename T>
		void SetVertexFromIndex(size_t i, T&& val) NOEXCEPTRELEASE
		{
			const auto& info = vertexLayout.GetVertexInfoFromIndex(i);
			auto vertex = data + info.GetOffset();

			switch (info.GetType())
			{
			case VertexLayout::Position2D:
				SetVertexType<VertexLayout::Position2D>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::Position3D:
				SetVertexType<VertexLayout::Position3D>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::Texture2D:
				SetVertexType<VertexLayout::Texture2D>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::Normal:
				SetVertexType<VertexLayout::Normal>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::Tangent:
				SetVertexType<VertexLayout::Tangent>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::BiTangent:
				SetVertexType<VertexLayout::BiTangent>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::ColorFloat3:
				SetVertexType<VertexLayout::ColorFloat3>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::ColorFloat4:
				SetVertexType<VertexLayout::ColorFloat4>(vertex, std::forward<T>(val));
				break;

			case VertexLayout::ARBGColor:
				SetVertexType<VertexLayout::ARBGColor>(vertex, std::forward<T>(val));
				break;

			default:
				assert("해당 Vertex Type은 존재하지 않음" && false);
			}
		}

	protected:
		Vertex(char* data, const VertexLayout& vertexLayout) NOEXCEPTRELEASE;

	private:
		// 여러개 정점 타입을 받고 재귀를 통해서 한개씩 처리를 하고 그 한개는 
		template<typename First, typename ...Rest>
		void SetVertexFromIndex(size_t i, First&& first, Rest&&... rest) NOEXCEPTRELEASE
		{
			SetVertexFromIndex(i, std::forward<First>(first));
			SetVertexFromIndex(i + 1, std::forward<Rest>(rest)...);
		}

		template<VertexLayout::VertexType DestVertexType, typename SrcType>
		void SetVertexType(char* vertex, SrcType&& val) NOEXCEPTRELEASE
		{
			using Dest = typename VertexLayout::Map<DestVertexType>::Type;

			if constexpr (std::is_assignable<Dest, SrcType>::value)
				*reinterpret_cast<Dest*>(vertex) = val;

			else
				assert("변수 타입이 일치하지 않음" && false);
		}

		char* data = nullptr;
		const VertexLayout& vertexLayout;
	};

	// 정점 데이터를 봐꿀 수 없는 정점 (읽기 전용)
	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& vertex) NOEXCEPTRELEASE;


		template<VertexLayout::VertexType Type>
		const auto& GetValue() const NOEXCEPTRELEASE { return const_cast<Vertex&>(vertex).GetValue<Type>(); }

	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout vertexLayout, size_t size = 0u) NOEXCEPTRELEASE;

		const char* data() const NOEXCEPTRELEASE;
		const VertexLayout& GetVertexLayout() const noexcept;
		size_t count() const NOEXCEPTRELEASE;
		size_t size() const NOEXCEPTRELEASE;
		void Resize(size_t newSize) NOEXCEPTRELEASE;

		Vertex back() NOEXCEPTRELEASE;
		Vertex front() NOEXCEPTRELEASE;
		Vertex operator[](size_t i) NOEXCEPTRELEASE;

		ConstVertex back() const NOEXCEPTRELEASE;
		ConstVertex front() const NOEXCEPTRELEASE;
		ConstVertex operator[](size_t i) const NOEXCEPTRELEASE;

		template<typename ...Params>
		void emplace_back(Params&&... params) NOEXCEPTRELEASE
		{
			// 입력한 Vertex 타입 정보 개수와 저장된 정점 타입 개수가 맞는지 확인
			assert(sizeof...(params) == vertexLayout.count() && "입력한 정점 타입 개수가 추가한 Vertex Layout 개수와 일치하지 않음");

			// 추가되는 layout의 사이즈만큼 늘림
			buffer.resize(buffer.size() + vertexLayout.size());

			back().SetVertexFromIndex(0, std::forward<Params>(params)...);
		}

	private:
		std::vector<char> buffer;	// Vertex Buffer의 데이터
		VertexLayout vertexLayout;	// Vertex Buffer의 Vertex Layout
	};
}