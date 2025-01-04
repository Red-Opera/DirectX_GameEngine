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

			size_t GetNextOffset() const NOEXCEPTRELEASE;					// �� ������ �������� �� ���� ������ ��ġ�� ��Ÿ���� �޼ҵ�
			size_t GetOffset() const;										// �� �����Ͱ� �����ϴ� ��ġ
			size_t size() const NOEXCEPTRELEASE;							// �� ������ Ÿ���� ũ��

			VertexType GetType() const noexcept;							// Ÿ�� ������ �������� �޼ҵ�
			D3D11_INPUT_ELEMENT_DESC GetInputDESC() const NOEXCEPTRELEASE;	// Input Layout�� DESC�� �������� �޼ҵ�

			const char* GetCode() const noexcept;

			// �Էµ� VertexType�� �ٶ� ���� �޸� ũ�⸦ ������ Ÿ�ӿ� ��ȯ���ִ� �Լ�
			static const constexpr size_t GetTypeSize(VertexType type) NOEXCEPTRELEASE;

		private:
			// Input Layout DESC�� ������ִ� �޼ҵ�
			template<VertexType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC CreateInputDESC(size_t offset) noexcept
			{
				return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

			VertexType type;	// ���� Ÿ��
			size_t offset;		// �ش� ���� Ÿ���� ���� �޸� ��ġ
		};

		// ���� ���� Ÿ���� ����Ǿ� �ִ� ������ �������� �޼ҵ�
		template<VertexType Type>
		const VertexInfo GetVertexInfo() const NOEXCEPTRELEASE
		{
			// �ش� Ÿ���� ����
			for (auto& info : vertexInfo)
			{
				if (info.GetType() == Type)
					return info;
			}

			assert("��ϵ� ���� ������ �������� �ʽ��ϴ�." && false);

			return vertexInfo.front();
		}

		const VertexInfo& GetVertexInfoFromIndex(size_t i) const NOEXCEPTRELEASE;	// �ε����� ���ؼ� ���� Ÿ���� �������� �޼ҵ�
		VertexLayout& AddType(VertexType type) NOEXCEPTRELEASE;						// ���� Ÿ�Կ� �ش� Ÿ���� ������ �߰��ϴ� �޼ҵ�

		// ���� Ÿ�Ե��� �� ũ��, ���� ����
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
		// �ش� Ÿ�Կ� �´� Vertex �������� Type�� �´� ���� ��ȯ��
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
				assert("�ش� Vertex Type�� �������� ����" && false);
			}
		}

	protected:
		Vertex(char* data, const VertexLayout& vertexLayout) NOEXCEPTRELEASE;

	private:
		// ������ ���� Ÿ���� �ް� ��͸� ���ؼ� �Ѱ��� ó���� �ϰ� �� �Ѱ��� 
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
				assert("���� Ÿ���� ��ġ���� ����" && false);
		}

		char* data = nullptr;
		const VertexLayout& vertexLayout;
	};

	// ���� �����͸� ���� �� ���� ���� (�б� ����)
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
			// �Է��� Vertex Ÿ�� ���� ������ ����� ���� Ÿ�� ������ �´��� Ȯ��
			assert(sizeof...(params) == vertexLayout.count() && "�Է��� ���� Ÿ�� ������ �߰��� Vertex Layout ������ ��ġ���� ����");

			// �߰��Ǵ� layout�� �����ŭ �ø�
			buffer.resize(buffer.size() + vertexLayout.size());

			back().SetVertexFromIndex(0, std::forward<Params>(params)...);
		}

	private:
		std::vector<char> buffer;	// Vertex Buffer�� ������
		VertexLayout vertexLayout;	// Vertex Buffer�� Vertex Layout
	};
}