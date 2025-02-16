#pragma once

#include <vector>
#include <type_traits>

#include <DirectXMath.h>
#include <minwindef.h>

#include <assimp/scene.h>
#include <utility>

#include "Core/DxGraphic.h"
#include "Core/Draw/Base/ColorInfo.h"

#define CREATE_GET_VERTEX_INFO(aiMeshMember) \
static Type get(const aiMesh& mesh, size_t i) noexcept { return *reinterpret_cast<const Type*>(&mesh.aiMeshMember[i]);}

#define LAYOUT_TYPES	\
	TYPE(Position2D)	\
	TYPE(Position3D)	\
	TYPE(Texture2D)		\
	TYPE(Normal)		\
	TYPE(Tangent)		\
	TYPE(BiTangent)		\
	TYPE(ColorFloat3)	\
	TYPE(ColorFloat4)	\
	TYPE(ARBGColor)		\
	TYPE(Count)

namespace VertexCore
{
	class VertexLayout
	{
	public:
		enum VertexType 
		{ 
#define TYPE(type) type,
			LAYOUT_TYPES
#undef TYPE
		};

		template<VertexType> struct Map;
		template<> struct Map<Position2D>
		{
			using Type = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;

			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "Position2D";

			CREATE_GET_VERTEX_INFO(mVertices)
		};

		template<> struct Map<Position3D>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "Position3D";

			CREATE_GET_VERTEX_INFO(mVertices)
		};

		template<> struct Map<Texture2D>
		{
			using Type = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;

			static constexpr const char* semantic = "TEXCOORD";
			static constexpr const char* code = "Texture2D";

			CREATE_GET_VERTEX_INFO(mTextureCoords[0])
		};

		template<> struct Map<Normal>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "Normal";

			CREATE_GET_VERTEX_INFO(mNormals)
		};

		template<> struct Map<Tangent>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Tangent";

			CREATE_GET_VERTEX_INFO(mTangents)
		};

		template<> struct Map<BiTangent>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "BiTangent";

			CREATE_GET_VERTEX_INFO(mBitangents)
		};

		template<> struct Map<ColorFloat3>
		{
			using Type = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color3D";

			CREATE_GET_VERTEX_INFO(mColors[0])
		};

		template<> struct Map<ColorFloat4>
		{
			using Type = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color4D";

			CREATE_GET_VERTEX_INFO(mColors[0])
		};

		template<> struct Map<ARBGColor>
		{
			using Type = ::ARBGColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Color4D8Bit";

			CREATE_GET_VERTEX_INFO(mColors[0])
		};

		template<> struct Map<Count>
		{
			using Type = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

			static constexpr const char* semantic = "ErrorType";
			static constexpr const char* code = "ErrorType";
			
			CREATE_GET_VERTEX_INFO(mFaces)
		};

		template<template<VertexLayout::VertexType> class VertexTypeTarget, typename... Args>
		static constexpr auto RunStructFunction(VertexLayout::VertexType vertexType, Args&&... data) NOEXCEPTRELEASE
		{
			switch (vertexType)
			{
#define TYPE(type)						\
				case VertexLayout::type:\
					return VertexTypeTarget<VertexLayout::type>::Run(std::forward<Args>(data)...);

				LAYOUT_TYPES
#undef TYPE
			}

			assert("�ش� Vertex Type�� �������� ����" && false);

			return VertexTypeTarget<VertexLayout::Count>::Run(std::forward<Args>(data)...);
		}

		class VertexInfo
		{
		public:
			VertexInfo(VertexType type, size_t offset);

			// �Էµ� VertexType�� �ٶ� ���� �޸� ũ�⸦ ������ Ÿ�ӿ� ��ȯ���ִ� �Լ�
			static const constexpr size_t GetTypeSize(VertexType type) NOEXCEPTRELEASE;

			size_t GetNextOffset() const NOEXCEPTRELEASE;					// �� ������ �������� �� ���� ������ ��ġ�� ��Ÿ���� �޼ҵ�
			size_t GetOffset() const;										// �� �����Ͱ� �����ϴ� ��ġ
			size_t size() const NOEXCEPTRELEASE;							// �� ������ Ÿ���� ũ��

			VertexType GetType() const noexcept;							// Ÿ�� ������ �������� �޼ҵ�
			D3D11_INPUT_ELEMENT_DESC GetInputDESC() const NOEXCEPTRELEASE;	// Input Layout�� DESC�� �������� �޼ҵ�

			const char* GetCode() const noexcept;

		private:
			// Input Layout DESC�� ������ִ� �޼ҵ�
			//template<VertexType type>
			//static constexpr D3D11_INPUT_ELEMENT_DESC CreateInputDESC(size_t offset) noexcept
			//{
			//	return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			//}

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

		bool has(VertexType type) const noexcept;

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

			VertexLayout::RunStructFunction<SetVertexTypeSetting>(info.GetType(), this, vertex, std::forward<T>(val));
		}

	protected:
		Vertex(char* data, const VertexLayout& vertexLayout) NOEXCEPTRELEASE;

	private:
		template<VertexLayout::VertexType type>
		struct SetVertexTypeSetting
		{
			template<typename T>
			static constexpr auto Run(Vertex* vertex, char* vertexData, T&& data) NOEXCEPTRELEASE
			{
				return vertex->SetVertexType<type>(vertexData, std::forward<T>(data));
			}
		};

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
		VertexBuffer(VertexLayout vertexLayout, const aiMesh& mesh);

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

#undef CREATE_GET_VERTEX_INFO
#ifndef USE_VERTEX_CODE
#undef LAYOUT_TYPES
#endif