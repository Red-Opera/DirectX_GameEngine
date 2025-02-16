#pragma once

#include "Core/Exception/WindowException.h"

#include <DirectXMath.h>
#include <cassert>
#include <vector>
#include <memory>
#include <string>
#include <optional>

#define CHILD_TYPES			\
		TYPE_NAME(float1)	\
		TYPE_NAME(float2)	\
		TYPE_NAME(float3)	\
		TYPE_NAME(float4)	\
		TYPE_NAME(Matrix)	\
		TYPE_NAME(Bool)		\
		TYPE_NAME(int32)	\


namespace DynamicConstantBuffer
{
	// HLSL 파일에서 사용할 수 있는 버퍼 타입
	enum BufferType 
	{
#define TYPE_NAME(childType) childType,
		CHILD_TYPES
#undef TYPE_NAME

		Struct, Array, Empty
	};

	template<BufferType type>
	struct BufferTypeInfo { static constexpr bool hasExistType = false; };		// 잘못된 타입인 경우

	template<> struct BufferTypeInfo<float1>
	{
		using SystemType = float;								// 기본 사용 타입
		static constexpr size_t hlslSize = sizeof(SystemType);	// HLSL 파일에서 사용되는 타입 사이즈
		static constexpr const char* code = "FLOAT1";			// 서명 만들 때 사용하는 코드
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<float2>
	{
		using SystemType = DirectX::XMFLOAT2;
		static constexpr size_t hlslSize = sizeof(SystemType);
		static constexpr const char* code = "FLOAT2";
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<float3>
	{
		using SystemType = DirectX::XMFLOAT3;
		static constexpr size_t hlslSize = sizeof(SystemType);
		static constexpr const char* code = "FLOAT3";
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<float4>
	{
		using SystemType = DirectX::XMFLOAT4;
		static constexpr size_t hlslSize = sizeof(SystemType);
		static constexpr const char* code = "FLOAT4";
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<Matrix>
	{
		using SystemType = DirectX::XMFLOAT4X4;
		static constexpr size_t hlslSize = sizeof(SystemType);
		static constexpr const char* code = "MATRIX4";
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<Bool>
	{
		using SystemType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr const char* code = "BOOL";
		static constexpr bool hasExistType = true;
	};

	template<> struct BufferTypeInfo<int32>
	{
		using SystemType = int;
		static constexpr size_t hlslSize = sizeof(SystemType);
		static constexpr const char* code = "INT";
		static constexpr bool hasExistType = true;
	};

#define TYPE_NAME(childType) \
	static_assert(BufferTypeInfo<childType>::hasExistType, #childType " 타입은 HLSL에서 사용할 수 없음");

	CHILD_TYPES
#undef TYPE_NAME

	template<typename T>
	struct SystemTypeInfo { static constexpr bool hasExistType = false; };

#define TYPE_NAME(childType)															\
	template<> struct SystemTypeInfo<typename BufferTypeInfo<childType>::SystemType>	\
	{																					\
		static constexpr BufferType bufferType = childType;								\
		static constexpr bool hasExistType = true;										\
	};

	CHILD_TYPES
#undef TYPE_NAME



	class LayoutType
	{
	public:
		std::string GetSignature() const NOEXCEPTRELEASE;

		bool IsExist() const noexcept;

		// [] 연산자는 구조체에서만 이름을 통해 접근할 수 있도록 함
		LayoutType& operator[](const std::string& name) NOEXCEPTRELEASE;
		const LayoutType& operator[](const std::string& name) const NOEXCEPTRELEASE;

		// 배열의 인덱스에 따라서 LayoutType을 반환하는 메소드
		std::pair<size_t, const LayoutType*> GetLayoutTypeFromArray(size_t offset, size_t index) const NOEXCEPTRELEASE;

		// T()는 배열에서만 동작하도록 함
		LayoutType& T() NOEXCEPTRELEASE;
		const LayoutType& T() const NOEXCEPTRELEASE;

		size_t GetOffsetBegin() const noexcept;
		size_t GetOffsetEnd() const noexcept;
		size_t GetLayoutByteSize() const noexcept;

		// Struct 전용 Type add 함수
		LayoutType& add(BufferType addType, std::string bufferName) NOEXCEPTRELEASE;
		
		template<BufferType addType>
		LayoutType& add(const std::string bufferName) NOEXCEPTRELEASE { return add(addType, std::move(bufferName)); }

		// 배열 전용 Type set 함수
		LayoutType& set(BufferType addType, size_t size) NOEXCEPTRELEASE;

		template<BufferType setType>
		LayoutType& set(size_t size) NOEXCEPTRELEASE { return set(setType, size); }

		template<typename T>
		size_t get() const NOEXCEPTRELEASE
		{
			switch (bufferType)
			{
#define TYPE_NAME(childType) case childType : assert(typeid(BufferTypeInfo<childType>::SystemType) == typeid(T)); return *offset;
				CHILD_TYPES
#undef TYPE_NAME

			default:
				assert("해당 타입은 HLSL에서 사용할 수 없음");
				return 0u;
			}
		}

	private:
		struct CompoundDataBase { virtual ~CompoundDataBase() = default; };

		friend class EditLayout;
		friend struct CompoundData;

		static LayoutType& GetEmptyType() noexcept { static LayoutType empty{}; return empty; }

		static size_t Get16ByteMemoryBlockBoundary(size_t offset) NOEXCEPTRELEASE;							// 현재 전체 16 Byte 메모리 블록 크기를 구함
		static bool IsAfterInsertOver16ByteMemoryBlock(size_t offset, size_t size) NOEXCEPTRELEASE;			// 데이터 삽입 후 16 Byte 메모리 블록이 넘어가는지 체크
		static size_t GetAfterInsert16ByteMemoryBlockBoundary(size_t offset, size_t size) NOEXCEPTRELEASE;	// 데이터 삽입 후 전체 16 Byte 메모리 블록 크기를 구함
		static bool TypeNameIsValidate(const std::string& name) noexcept;

		LayoutType() noexcept = default;
		LayoutType(BufferType bufferType) NOEXCEPTRELEASE;

		std::string GetSignatureForStruct() const NOEXCEPTRELEASE;
		std::string GetSignatureForArray() const NOEXCEPTRELEASE;

		size_t Finalize(size_t offset) NOEXCEPTRELEASE;

		size_t FinalizeForStruct(size_t offset);
		size_t FinalizeForArray(size_t offset);

		std::optional<size_t> offset;
		BufferType bufferType = Empty;
		std::unique_ptr<CompoundDataBase> compoundData;
	};

	class Layout
	{
		friend class TypeCache;
		friend class Buffer;

	public:
		size_t GetLayoutByteSize() const noexcept;
		std::string GetSignature() const NOEXCEPTRELEASE;

	protected:
		Layout(std::shared_ptr<LayoutType> layout) noexcept;

		std::shared_ptr<LayoutType> layout;
	};

	class EditLayout : public Layout
	{
		friend class TypeCache;
	public:
		EditLayout() noexcept;
		LayoutType& operator[] (const std::string& name) NOEXCEPTRELEASE;	// []를 이용하여 해당 타입을 가져옴

		template<BufferType bufferType>
		LayoutType& add(const std::string& name) NOEXCEPTRELEASE
		{
			return layout->add<bufferType>(name);
		}

	private:
		std::shared_ptr<LayoutType> GetLayout() noexcept;
		void ClearLayout() noexcept;
	};

	class CompleteLayout : public Layout
	{
		friend class TypeCache;
		friend class Buffer;

	public:
		const LayoutType& operator[](const std::string& name) const NOEXCEPTRELEASE;

		std::shared_ptr<LayoutType> GetLayout() const noexcept;

	private:
		CompleteLayout(std::shared_ptr<LayoutType> layout) noexcept;
		std::shared_ptr<LayoutType> RemoveLayout() const noexcept;
	};

	class TypeConstReference
	{
		friend class Buffer;
		friend class TypeReference;

	public:
		class TypePointer
		{
			friend TypeConstReference;
		public:
			template<typename T>
			operator const T* () const NOEXCEPTRELEASE
			{
				static_assert(SystemTypeInfo<std::remove_const_t<T>>::hasExistType, "포인터 변한에 사용되지 않는 시스템 타입");
				return &static_cast<const T&>(*ref);
			}

		private:
			TypePointer(const TypeConstReference* ref) noexcept;

			const TypeConstReference* ref;
		};

		bool IsExist() const noexcept;

		TypeConstReference operator[](const std::string& key) NOEXCEPTRELEASE;
		TypeConstReference operator[](size_t index) NOEXCEPTRELEASE;
		TypePointer operator&() NOEXCEPTRELEASE;

		template<typename T>
		operator const T& () const NOEXCEPTRELEASE
		{
			static_assert(SystemTypeInfo<std::remove_const_t<T>>::hasExistType, "포인터 변한에 사용되지 않는 시스템 타입");
			return *reinterpret_cast<const T*>(bytes + offset + layout->get<T>());
		}

	private:
		TypeConstReference(const LayoutType* layout, const char* bytes, size_t offset) noexcept;

		const LayoutType* layout;
		const char* bytes;

		size_t offset;
	};

	class TypeReference
	{
		friend class Buffer;

	public:
		class TypePointer
		{
			friend TypeReference;
		public:
			template<typename T>
			operator T* () const NOEXCEPTRELEASE
			{
				static_assert(SystemTypeInfo<std::remove_const_t<T>>::hasExistType, "포인터 변한에 사용되지 않는 시스템 타입");
				return &static_cast<T&>(*ref);
			}

		private:
			TypePointer(TypeReference* ref) noexcept;

			TypeReference* ref;
		};

		bool IsExist() const noexcept;

		operator TypeConstReference() const noexcept;
		TypeReference operator[](const std::string& key) NOEXCEPTRELEASE;
		TypeReference operator[](size_t index) NOEXCEPTRELEASE;
		TypePointer operator&() NOEXCEPTRELEASE;

		template<typename ChildType>
		bool SetValue(const ChildType& value) NOEXCEPTRELEASE
		{
			if (IsExist())
			{
				*this = value;

				return true;
			}

			return false;
		}

		template<typename T>
		operator T& () const NOEXCEPTRELEASE
		{
			static_assert(SystemTypeInfo<std::remove_const_t<T>>::hasExistType, "포인터 변한에 사용되지 않는 시스템 타입");
			return *reinterpret_cast<T*>(bytes + offset + layout->get<T>());
		}

		template<typename T>
		T& operator=(const T& rhs) const NOEXCEPTRELEASE
		{
			static_assert(SystemTypeInfo<std::remove_const_t<T>>::hasExistType, "포인터 변한에 사용되지 않는 시스템 타입");
			return static_cast<T&>(*this) = rhs;
		}

	private:
		TypeReference(const LayoutType* layout, char* bytes, size_t offset) noexcept;

		const LayoutType* layout;
		char* bytes;

		size_t offset;
	};

	class Buffer
	{
	public:
		Buffer(const Buffer&) noexcept;
		Buffer(const CompleteLayout& layout) NOEXCEPTRELEASE;

		Buffer(CompleteLayout&& layout) NOEXCEPTRELEASE;
		Buffer(EditLayout&& layout) NOEXCEPTRELEASE;
		Buffer(Buffer&&) noexcept;

		const char* data() const noexcept;
		size_t size() const noexcept;
		const LayoutType& GetLayout() const noexcept;
		std::shared_ptr<LayoutType> GetSharedLayout() const noexcept;

		void set(const Buffer&) NOEXCEPTRELEASE;

		TypeReference operator[](const std::string& key) NOEXCEPTRELEASE;
		TypeConstReference operator[](const std::string& key) const NOEXCEPTRELEASE;

	private:
		std::shared_ptr<LayoutType> layout;
		std::vector<char> bytes;
	};
}

#ifndef DYNAMIC_CONSTANT_BUFFER
#undef CHILD_TYPES
#endif // !CHILD_TYPES