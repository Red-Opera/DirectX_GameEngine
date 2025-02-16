#pragma once

#define DYNAMIC_CONSTANT_BUFFER
#include "DynamicConstantBuffer.h"
#include "TypeCache.h"

#include <algorithm>
#include <cctype>
#include <string>

// Get___���� �ش� Ÿ�Կ� �´� ���� ��ȯ�ϴ� ��ũ��
//#define CREATE_GETMETHOD(childType)							\
//size_t LayoutType::Get ## childType() const NOEXCEPTRELEASE	\
//{															\
//	assert(false && "Cannot get type to " #childType);		\
//															\
//	return 0u;												\
//}
//
//// LayoutType�� Ȱ���Ͽ� Layout ���� Ÿ���� ����� ��ũ��
//#define CHILD_TYPE_EX(childType, systemType, hlslSize)											\
//size_t childType::Get ## childType() const NOEXCEPTRELEASE { return GetOffsetBegin(); }			\
//size_t childType::GetOffsetEnd() const noexcept { return GetOffsetBegin() + SystemTypeSize(); }	\
//																								\
//size_t childType::Finalize(size_t offsetIn) NOEXCEPTRELEASE										\
//{																								\
//	offset = offsetIn;																			\
//	return offsetIn + sizeof(systemType);														\
//}																								\
//																								\
//size_t childType::SystemTypeSize() const NOEXCEPTRELEASE { return (hlslSize); }					\
//std::string childType::GetSignature() const NOEXCEPTRELEASE { return #childType; }
//
//#define CHILD_TYPE(childType, systemType) CHILD_TYPE_EX(childType, systemType, sizeof(systemType))
//
//// Ư�� ���(childType)�� �ý��� Ÿ��(systemType)���� ��ȯ�ϴ� ���
//#define CONVERT_REF_SYSTYPE(referenceType, childType, ...)											\
//																									\
///* ��ȯ ������: childType�� systype ������ ��ȯ (DirectX::XMFLOAT3& v = obj) */							\
//referenceType::operator __VA_ARGS__ childType::SystemType&() NOEXCEPTRELEASE						\
//{																									\
//	return *reinterpret_cast<childType::SystemType*>(bytes + offset + type->Get ## childType());	\
//}
//
//#define REF_ASSIGN(referenceType, childType)														\
//																									\
///* ���� ������ �����ε�: childType�� systype ������ ���� �����ϰ� �� */									\
//childType::SystemType& referenceType::operator=(const childType::SystemType& rhs) NOEXCEPTRELEASE	\
//{																									\
//	return static_cast<childType::SystemType&>(*this) = rhs;										\
//}
//
//#define REF_NONCONST(referenceType, childType) CONVERT_REF_SYSTYPE(referenceType, childType) REF_ASSIGN//(referenceType, childType)
//#define REF_CONST(referenceType, childType) CONVERT_REF_SYSTYPE(referenceType, childType, const)
//
//// Ư�� ���(childType)�� �ý��� ������ Ÿ��(systemType) ���� ��ȯ�ϴ� ���
//// DirectX::XMFLOAT3* ptr = obj; 
//#define CONVERT_POINT_SYSTYPE(systemType, childType, ...)								\
//systemType::TypePointer::operator __VA_ARGS__ childType::SystemType*() NOEXCEPTRELEASE	\
//{																						\
//	return &static_cast<__VA_ARGS__ childType::SystemType&>(ref);						\
//}

namespace DynamicConstantBuffer
{
	struct CompoundData
	{
		struct Struct : public LayoutType::CompoundDataBase 
		{ 
			std::vector<std::pair<std::string, LayoutType>> structData; 
		};

		struct Array : public LayoutType::CompoundDataBase
		{
			std::optional<LayoutType> layoutType;
			size_t typeSize;
			size_t size;
		};
	};

	std::string LayoutType::GetSignature() const NOEXCEPTRELEASE
	{
		switch (bufferType)
		{
#define TYPE_NAME(childType) case childType: return BufferTypeInfo<childType>::code;
			CHILD_TYPES
#undef TYPE_NAME

		case Struct:
			return GetSignatureForStruct();

		case Array:
			return GetSignatureForArray();

		default:
			assert("�߸��� HLSL Ÿ��" && false);
			return "???";
		}
	}

	bool LayoutType::IsExist() const noexcept
	{
		return bufferType != Empty;
	}
	

	bool TypeNameIsValidate(const std::string& name) noexcept
	{
		bool isEmpty = name.empty(), isDigit = std::isdigit(name.front());
		bool isAlphaNumUnderBar = std::all_of(name.begin(), name.end(), [](char c) { return std::isalnum(c) || c == '_'; });

		return !isEmpty && !isDigit && isAlphaNumUnderBar;
	}

	// [] �����ڴ� ����ü������ �̸��� ���� ������ �� �ֵ��� ��
	LayoutType& LayoutType::operator[](const std::string& name) NOEXCEPTRELEASE
	{
		assert("Cannot access member on non Struct" && bufferType == Struct);

		for (auto& type : static_cast<CompoundData::Struct&>(*compoundData).structData)
		{
			if (type.first == name)
				return type.second;
		}

		return GetEmptyType();
	}

	const LayoutType& LayoutType::operator[](const std::string& name) const NOEXCEPTRELEASE
	{
		return const_cast<LayoutType&>(*this)[name];
	}

	std::pair<size_t, const LayoutType*> LayoutType::GetLayoutTypeFromArray(size_t offset, size_t index) const NOEXCEPTRELEASE
	{
		assert("�ش� HLSL Ÿ���� �迭�� �ƴ�" && bufferType == Array);
		const auto& arrayData = static_cast<CompoundData::Array&>(*compoundData);

		assert(index < arrayData.size || arrayData.typeSize);
		return { offset + arrayData.typeSize * index, &*arrayData.layoutType };
	}

	// T()�� �迭������ �����ϵ��� ��
	LayoutType& LayoutType::T() NOEXCEPTRELEASE
	{
		assert("T�� �迭�� ������ �� ����" && bufferType == Array);

		return *static_cast<CompoundData::Array&>(*compoundData).layoutType;
	}

	const LayoutType& LayoutType::T() const NOEXCEPTRELEASE
	{
		return const_cast<LayoutType&>(*this).T();
	}

	size_t LayoutType::GetOffsetBegin() const noexcept
	{
		return *offset;
	}

	size_t LayoutType::GetOffsetEnd() const noexcept
	{
		switch (bufferType)
		{
#define TYPE_NAME(childType) case childType: return *offset + BufferTypeInfo<childType>::hlslSize;
			CHILD_TYPES
#undef TYPE_NAME

		case Struct:
			{
				const auto& arrayData = static_cast<CompoundData::Struct&>(*compoundData);
				return Get16ByteMemoryBlockBoundary(arrayData.structData.back().second.GetOffsetEnd());
			}

		case Array:
		{
			const auto& arrayData = static_cast<CompoundData::Array&>(*compoundData);
			return *offset + Get16ByteMemoryBlockBoundary(arrayData.layoutType->GetLayoutByteSize()) * arrayData.size;
		}

		default:
			assert("�߸��� HLSL Ÿ��" && false);
			return 0u;
		}
	}

	size_t LayoutType::GetLayoutByteSize() const noexcept
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}

	LayoutType& LayoutType::add(BufferType addType, std::string bufferName) noexcept(!_DEBUG)
	{
		assert("Cannot access member on non Struct" && bufferType == Struct);
		assert("�ش� ���� �̸��� �������� ����" && TypeNameIsValidate(bufferName));

		auto& structData = static_cast<CompoundData::Struct&>(*compoundData);

		for (auto& type : structData.structData)
		{
			if (type.first == bufferName)
				assert("�̹� �ش� �̸��� ����ü�� �� ����" && false);
		}

		structData.structData.emplace_back(std::move(bufferName), LayoutType(addType));
		return *this;
	}

	LayoutType& LayoutType::set(BufferType addType, size_t size) NOEXCEPTRELEASE
	{
		assert("�ش� Ÿ���� �迭�� �ƴ�" && bufferType == Array);
		assert(size != 0u);

		auto& arrayData = static_cast<CompoundData::Array&>(*compoundData);
		arrayData.layoutType = { addType };
		arrayData.size = size;

		return *this;
	}

	size_t LayoutType::Get16ByteMemoryBlockBoundary(size_t offset) noexcept(!_DEBUG)
	{
		return offset + (16u - offset % 16u) % 16u;
	}

	bool LayoutType::IsAfterInsertOver16ByteMemoryBlock(size_t offset, size_t size) NOEXCEPTRELEASE
	{
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;

		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
	}

	size_t LayoutType::GetAfterInsert16ByteMemoryBlockBoundary(size_t offset, size_t size) NOEXCEPTRELEASE
	{
		return IsAfterInsertOver16ByteMemoryBlock(offset, size) ? Get16ByteMemoryBlockBoundary(offset) : offset;
	}

	bool LayoutType::TypeNameIsValidate(const std::string& name) noexcept
	{
		bool isEmpty = name.empty(), isDigit = std::isdigit(name.front());
		bool isAlphaNumUnderBar = std::all_of(name.begin(), name.end(), [](char c) { return std::isalnum(c) || c == '_'; });

		return !isEmpty && !isDigit && isAlphaNumUnderBar;
	}

	LayoutType::LayoutType(BufferType bufferType) NOEXCEPTRELEASE : bufferType(bufferType)
	{
		assert(bufferType != Empty);
		
		if (bufferType == Struct)
			compoundData = std::unique_ptr<CompoundData::Struct>{ new CompoundData::Struct() };


		else if (bufferType == Array)
			compoundData = std::unique_ptr<CompoundData::Array>{ new CompoundData::Array() };
	}

	std::string LayoutType::GetSignatureForStruct() const noexcept(!_DEBUG)
	{
		using namespace std::string_literals;
		auto signature = "Struct{"s;

		for (const auto& type : static_cast<CompoundData::Struct&>(*compoundData).structData)
			signature += type.first + ":"s + type.second.GetSignature() + ";"s;

		signature += "}"s;
		return signature;
	}

	std::string LayoutType::GetSignatureForArray() const noexcept(!_DEBUG)
	{
		using namespace std::string_literals;
		const auto& arrayData = static_cast<CompoundData::Array&>(*compoundData);

		return "Array:"s + std::to_string(arrayData.size) + "{"s + arrayData.layoutType->GetSignature() + "}"s;
	}

	size_t LayoutType::Finalize(size_t offset) noexcept(!_DEBUG)
	{
		switch (bufferType)
		{
#define TYPE_NAME(childType) case childType:																		\
			this->offset = GetAfterInsert16ByteMemoryBlockBoundary(offset, BufferTypeInfo<childType>::hlslSize);	\
			return *(this->offset) + BufferTypeInfo<childType>::hlslSize;

			CHILD_TYPES
#undef TYPE_NAME

		case Struct:
			return FinalizeForStruct(offset);

		case Array:
			return FinalizeForArray(offset);

		default:
			assert("�߸��� HLSL Ÿ��" && false);
			return 0u;
		}
	}

	size_t LayoutType::FinalizeForStruct(size_t offset)
	{
		auto& structData = static_cast<CompoundData::Struct&>(*compoundData);
		assert(structData.structData.size() != 0u);

		this->offset = Get16ByteMemoryBlockBoundary(offset);
		auto offsetNext = *(this->offset);

		for (auto& type : structData.structData)
			offsetNext = type.second.Finalize(offsetNext);

		return offsetNext;
	}

	size_t LayoutType::FinalizeForArray(size_t offset)
	{
		auto& arrayData = static_cast<CompoundData::Array&>(*compoundData);
		assert(arrayData.size != 0u);

		this->offset = Get16ByteMemoryBlockBoundary(offset);
		arrayData.layoutType->Finalize(*(this->offset));
		arrayData.typeSize = LayoutType::Get16ByteMemoryBlockBoundary(arrayData.layoutType->GetLayoutByteSize());

		return GetOffsetEnd();
	}

	Layout::Layout(std::shared_ptr<LayoutType> layout) noexcept : layout(std::move(layout)) {}

	size_t Layout::GetLayoutByteSize() const noexcept
	{
		return layout->GetLayoutByteSize();
	}

	std::string Layout::GetSignature() const NOEXCEPTRELEASE
	{
		return layout->GetSignature();
	}

	EditLayout::EditLayout() noexcept : Layout{ std::shared_ptr<LayoutType> { new LayoutType(Struct) } }
	{

	}

	LayoutType& EditLayout::operator[](const std::string& name) NOEXCEPTRELEASE
	{
		return (*layout)[name];
	}

	std::shared_ptr<LayoutType> EditLayout::GetLayout() noexcept
	{
		auto temp = std::move(layout);	// ���� layout�� temp�� �̵�
		temp->Finalize(0);				// temp ���̾ƿ��� �ʱ�ȭ

		*this = EditLayout();			// ���� EditLayout ��ü�� �ʱ�ȭ
		return std::move(temp);			// �ʱ�ȭ�� ���̾ƿ��� ��ȯ
	}

	void EditLayout::ClearLayout() noexcept
	{
		*this = EditLayout();
	}

	const LayoutType& CompleteLayout::operator[](const std::string& name) const NOEXCEPTRELEASE
	{
		return (*layout)[name];
	}

	std::shared_ptr<LayoutType> CompleteLayout::GetLayout() const noexcept
	{
		return layout;
	}

	CompleteLayout::CompleteLayout(std::shared_ptr<LayoutType> layout) noexcept : Layout(std::move(layout))
	{

	}

	std::shared_ptr<LayoutType> CompleteLayout::RemoveLayout() const noexcept
	{
		return std::move(layout);
	}

	TypeConstReference::TypePointer::TypePointer(const TypeConstReference* ref) noexcept : ref(ref) {}

	bool TypeConstReference::IsExist() const noexcept
	{
		return layout->IsExist();
	}

	TypeConstReference TypeConstReference::operator[](const std::string& name) NOEXCEPTRELEASE
	{
		return { &(*layout)[name], bytes, offset };
	}

	TypeConstReference TypeConstReference::operator[](size_t index) NOEXCEPTRELEASE
	{
		const auto type = layout->GetLayoutTypeFromArray(offset, index);

		return { type.second, bytes, type.first };
	}

	TypeConstReference::TypePointer TypeConstReference::operator&() NOEXCEPTRELEASE { return TypePointer{ this }; }

	TypeConstReference::TypeConstReference(const LayoutType* layout, const char* bytes, size_t offset) noexcept
		: layout(layout), bytes(bytes), offset(offset)
	{

	}

	TypeReference::TypePointer::TypePointer(TypeReference* ref) noexcept : ref(ref)
	{

	}

	bool TypeReference::IsExist() const noexcept
	{
		return layout->IsExist();
	}

	TypeReference::operator TypeConstReference() const noexcept { return { layout, bytes, offset }; }

	TypeReference TypeReference::operator[](const std::string& key) NOEXCEPTRELEASE
	{
		return { &(*layout)[key], bytes, offset };
	}

	TypeReference TypeReference::operator[](size_t index) NOEXCEPTRELEASE
	{
		const auto type = layout->GetLayoutTypeFromArray(offset, index);

		return { type.second, bytes, type.first };
	}

	TypeReference::TypePointer TypeReference::operator&() NOEXCEPTRELEASE { return TypePointer{ const_cast<TypeReference*>(this) }; }

	TypeReference::TypeReference(const LayoutType* layout, char* bytes, size_t offset) noexcept 
		: layout(layout), bytes(bytes), offset(offset)
	{

	}


	Buffer::Buffer(const Buffer& buffer) noexcept : layout(buffer.layout), bytes(buffer.bytes)
	{
	}

	Buffer::Buffer(const CompleteLayout& layout) NOEXCEPTRELEASE : layout(layout.GetLayout()), bytes(this->layout->GetOffsetEnd())
	{

	}

	Buffer::Buffer(CompleteLayout&& layout) NOEXCEPTRELEASE : layout(layout.GetLayout()), bytes(this->layout->GetOffsetEnd())
	{

	}

	Buffer::Buffer(EditLayout&& layout) NOEXCEPTRELEASE : Buffer(TypeCache::Get(std::move(layout)))
	{

	}

	Buffer::Buffer(Buffer&& buffer) noexcept : layout(std::move(buffer.layout)), bytes(std::move(buffer.bytes))
	{

	}

	const char* Buffer::data() const noexcept { return bytes.data(); }

	size_t Buffer::size() const noexcept { return bytes.size(); }

	const LayoutType& Buffer::GetLayout() const noexcept { return *layout; }

	std::shared_ptr<LayoutType> Buffer::GetSharedLayout() const noexcept { return layout; }

	void Buffer::set(const Buffer& other) NOEXCEPTRELEASE
	{
		assert(&GetLayout() == &other.GetLayout());

		std::copy(other.bytes.begin(), other.bytes.end(), bytes.begin());
	}


	TypeReference Buffer::operator[](const std::string& key) NOEXCEPTRELEASE
	{
		return { &(*layout)[key], bytes.data(), 0u };
	}

	TypeConstReference Buffer::operator[](const std::string& key) const NOEXCEPTRELEASE
	{
		return const_cast<Buffer&>(*this)[key];
	}
}