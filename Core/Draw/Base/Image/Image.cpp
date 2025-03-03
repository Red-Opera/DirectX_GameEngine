#include "stdafx.h"
#include "Image.h"

#include "Core/Window.h"

#include "Utility/StringConverter.h"

#include <windows.h>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;

namespace GraphicResource
{
	Image::Exception::Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr) noexcept
		: BaseException(line, file), note(move(note))
	{
		if (hr)
			this->note = "[Error String] : " + Window::Exception::TranslateErrorCode(*hr) + "\n" + this->note;
	}

	Image::Exception::Exception(int line, const char* file, std::string fileName, std::string noteIn, std::optional<HRESULT> hr) noexcept
		: BaseException(line, file)
	{
		using namespace std::string_literals;
		note = "[File Name] : "s + fileName + "\n"s + "[Note] "s + noteIn;

		if (hr)
			note = "[Error String] : " + Window::Exception::TranslateErrorCode(*hr) + note;
	}

	const char* Image::Exception::what() const noexcept
	{
		ostringstream out;

		out << BaseException::what() << endl << GetNote();
		exceptionText = out.str();

		return exceptionText.c_str();
	}

	const char* Image::Exception::GetType() const noexcept
	{
		return "�׷��� ���� ����";
	}

	const string& Image::Exception::GetNote() const noexcept
	{
		return note;
	}

	Image::Image(UINT width, UINT height)
	{
		// �ʱ� ��Ƽ���� �̹��� �⺻ ������ ���� (�Ӹ� ���� 1, 1¥��)
		HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);

		if (FAILED(hr))
			throw Image::Exception(__LINE__, __FILE__, "��Ƽ���� �̹����� �ʱ�ȭ�� �� ���� (�ʺ� : " + to_string(width) + ", ���� : " + to_string(height) + ")");
	}

	void Image::Clear(Color fillColor) noexcept
	{
		const auto width = GetWidth();
		const auto height = GetHeight();

		auto& imageData = *scratch.GetImage(0, 0, 0);

		for (size_t y = 0; y < height; y++)
		{
			// ���� ���� ��ġ�� ����
			auto rowStart = reinterpret_cast<Color*>(imageData.pixels + imageData.rowPitch * y);

			// ���� ���ۺ��� ���� ������ ���� ä��
			std::fill(rowStart, rowStart + imageData.width, fillColor);
		}
	}

	void Image::SetColorPixel(UINT x, UINT y, Color c) NOEXCEPTRELEASE
	{
		assert("������ �ȼ��� ��ġ�� ������ϴ�!" && x >= 0 && y >= 0 && x < GetWidth() && y < GetHeight());

		auto& imageData = *scratch.GetImage(0, 0, 0);
		reinterpret_cast<Color*>(&imageData.pixels[y * imageData.rowPitch])[x] = c;
	}

	Image::Color Image::GetColorPixel(UINT x, UINT y) const NOEXCEPTRELEASE
	{
		assert("������ �ȼ��� ��ġ�� ������ϴ�!" && x >= 0 && y >= 0 && x < GetWidth() && y < GetHeight());

		auto& imageData = *scratch.GetImage(0, 0, 0);
		return reinterpret_cast<Color*>(&imageData.pixels[y * imageData.rowPitch])[x];
	}

	UINT Image::GetWidth() const noexcept
	{
		return (UINT)scratch.GetMetadata().width;
	}

	UINT Image::GetHeight() const noexcept
	{
		return (UINT)scratch.GetMetadata().height;
	}

	UINT Image::GetBytePitch() const noexcept
	{
		return (UINT)scratch.GetImage(0, 0, 0)->rowPitch;
	}

	Image::Color* Image::get() noexcept
	{
		return reinterpret_cast<Color*>(scratch.GetPixels());
	}

	const Image::Color* Image::get() const noexcept
	{
		return const_cast<Image*>(this)->get();
	}

	const Image::Color* Image::GetConst() const noexcept
	{
		return const_cast<Image*>(this)->get();
	}

	Image Image::FromFile(const std::string& name)
	{
		// DDS ���� ��ȯ �� ������ ��ġ, DDS ������ ��ü ���
		std::wstring tempDDSFolder = L"Temp/TextureCache/";
		std::wstring tempDDSPath = tempDDSFolder + StringConverter::ToWString((StringConverter::GetFileName(name))) + L".dds";

		// �̹��� ������ �ε���
		DirectX::ScratchImage scratch;
		HRESULT hr;

		// �ӽ� ������ �����ϴ��� Ȯ���ϰ� ������ ����
		if (!std::filesystem::exists(tempDDSFolder))
		{
			std::filesystem::create_directories(tempDDSFolder);
			hr = E_FAIL;
		}

		// �̹� DDS�� ĳ�� �ؽ�ó�� ������ �� �̹��� ���
		else
			hr = LoadTempDDSTexture(tempDDSPath, scratch);

		// ĳ�� �ؽ�ó�� ���� ���� ������ �ϴ� ���
		if (FAILED(hr))
		{
			hr = DirectX::LoadFromWICFile(StringConverter::ToWide(name).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch);

			// ���� ���� �̹����� ������
			if (SUCCEEDED(hr))
			{
				HRESULT saveHr = SaveToDDSFile(
					scratch.GetImages(),
					scratch.GetImageCount(),
					scratch.GetMetadata(),
					DDS_FLAGS_NONE,
					tempDDSPath.c_str()
				);

				if (FAILED(saveHr))
				{
					std::stringstream out;
					out << "[" << StringConverter::ToString(tempDDSPath) << "]�� ��η� ������ �� ����!";

					throw Image::Exception(__LINE__, __FILE__, out.str());
				}
			}
		}

		if (FAILED(hr))
		{
			std::stringstream out;
			out << name << "�� ������ �ε��� �� ����!";

			throw Image::Exception(__LINE__, __FILE__, out.str());
		}

		// �̹����� ������ �������� �ʴ� ���
		if (scratch.GetImage(0, 0, 0)->format != format)
		{
			DirectX::ScratchImage converted;
			hr = DirectX::Convert(
				*scratch.GetImage(0, 0, 0),
				format,
				DirectX::TEX_FILTER_DEFAULT,
				DirectX::TEX_THRESHOLD_DEFAULT,
				converted);

			if (FAILED(hr))
			{
				std::stringstream out;
				out << name << "�� ������ " << format << "���� ��ȯ�� �� ����!";

				throw Image::Exception(__LINE__, __FILE__, out.str());
			}

			return Image(std::move(converted));
		}

		return Image(std::move(scratch));
	}

	void Image::Save(const string& fileName) const
	{
		const auto GetCodecID = [](const std::string& fileName)
			{
				const std::filesystem::path path = fileName;
				const auto extenstionPath = path.extension().string();

				if (extenstionPath == ".png")
					return DirectX::WIC_CODEC_PNG;

				else if (extenstionPath == ".jpg")
					return DirectX::WIC_CODEC_JPEG;

				else if (extenstionPath == ".bmp")
					return DirectX::WIC_CODEC_BMP;

				throw Exception(__LINE__, __FILE__, fileName, "��� �̹��� Ȯ���ڰ� �������� ����");
			};

		HRESULT hr = DirectX::SaveToWICFile(*scratch.GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, GetWICCodec(GetCodecID(fileName)), StringConverter::ToWide(fileName).c_str());

		if (FAILED(hr))
		{
			std::stringstream out;
			out << fileName << "�̸��� ������ ������ �� ����.";

			throw Exception(__LINE__, __FILE__, out.str());
		}
	}

	bool Image::HasAlpha() const noexcept
	{
		const uint8_t* data = scratch.GetPixels();
		size_t size = GetWidth() * GetHeight();

		for (size_t i = 0; i < size; i++)
		{
			if (data[i * 4 + 3] != 255)
				return true;
		}

		return false;
	}

	HRESULT Image::LoadTempDDSTexture(const std::wstring& filePath, DirectX::ScratchImage& image)
	{
		HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (hFile == INVALID_HANDLE_VALUE)
			return E_FAIL;

		DWORD fileSize = GetFileSize(hFile, nullptr);

		if (fileSize == INVALID_FILE_SIZE)
		{
			CloseHandle(hFile);
			
			std::stringstream out;
			out << "[" << StringConverter::ToString(filePath) << "] ����� ������ �߸��� ���� ������";

			throw Exception(__LINE__, __FILE__, out.str());

			return E_FAIL;
		}

		HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);

		if (!hMapping)
		{
			CloseHandle(hFile);

			std::stringstream out;
			out << "[" << StringConverter::ToString(filePath) << "] ����� ���� ���� ����!";

			throw Exception(__LINE__, __FILE__, out.str());

			return E_FAIL;
		}

		LPVOID data = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, fileSize);

		if (!hMapping || data == nullptr)
		{
			CloseHandle(hFile);
			CloseHandle(hMapping);

			std::stringstream out;
			out << "[" << StringConverter::ToString(filePath) << "] ����� ���� ���� �б� ����";

			throw Exception(__LINE__, __FILE__, out.str());

			return E_FAIL;
		}

		HRESULT hr = LoadFromDDSMemory(reinterpret_cast<const std::byte*>(data), fileSize, DDS_FLAGS_NONE, nullptr, image);

		UnmapViewOfFile(data);
		CloseHandle(hMapping);
		CloseHandle(hFile);

		return hr;
	}

	Image::Image(DirectX::ScratchImage scratch) noexcept : scratch(std::move(scratch))
	{

	}

	DirectX::XMVECTOR Image::Color::ConvertVector(Image::Color color)
	{
		using namespace DirectX;

		auto vector = XMVectorSet((float)color.GetR(), (float)color.GetG(), (float)color.GetB(), 0.0f);
		vector = vector * XMVectorReplicate(2.0f / 255.0f);
		vector = vector - Vector::allDirV;

		return vector;
	}
}